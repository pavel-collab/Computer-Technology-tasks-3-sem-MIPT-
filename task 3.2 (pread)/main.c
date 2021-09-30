//* Compile with gcc -Wall -Wextra -o out main.c

// go to man 2 pwrite
#define _XOPEN_SOURCE 500

#include <stdint.h> // uint8_t

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include <assert.h>

const unsigned int MAX_LEN = 256;

#include "../enum.h"

ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = buf;

    while (bytes_written < count) {
        ssize_t res = pwrite(fd, buf_addr + bytes_written, count - bytes_written, 0);

        if (res < 0) {
            return res;
        }

        bytes_written += (size_t) res;
    }
    return (ssize_t) bytes_written;
}

// the function of the removing fire from the current dir
int rm_file(char* filename) {

    // конкатенация строк
    const char *rm  = "rm ";

    char command[512];
    snprintf(command, sizeof command, "%s%s", rm, filename);
    // ----------------------------------------------------------------------------------------

    system(command);
    printf("[+] Successful removing: %s", filename);

    return 0;
}

int copy_file(char* copy_file, char* destination_file) {

    int cp_file   = open(copy_file, O_RDONLY | __O_LARGEFILE);
    if (cp_file < 0) {
        perror("Failed for open copy file for writing");
        rm_file(destination_file);
        return RESULT_OPEN_FAILED;
    }

    int dstn_file = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
    if (dstn_file < 0) {
        perror("Failed for open destination file for writing");
        rm_file(destination_file);
        return RESULT_OPEN_FAILED;
    }

    struct stat sb = {};

    if (lstat(copy_file, &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // узнаем размер файла в байтах
    long long copy_file_size = (long long) sb.st_size;

    char* buf = (char*) calloc(MAX_LEN, sizeof(char));
    assert(buf != NULL);


    while(copy_file_size > 0) {

        off_t position = 0;
        unsigned int rsym_count = 0;
        if (copy_file_size > MAX_LEN) {
            position = copy_file_size - MAX_LEN;
            rsym_count = MAX_LEN;
        }
        else {
            position = 0;
            rsym_count = copy_file_size;
        }
        
        ssize_t read_symb_amount = pread(cp_file, buf, rsym_count, position);

        if (read_symb_amount < 0) {
            perror("Failed read from the file");
            rm_file(destination_file);
            close(cp_file);
            return RESULT_BAD_READ;
        }

        ssize_t write_symb_amount = write(dstn_file, buf, read_symb_amount);

        if (write_symb_amount < 0) {
            perror("Failed write to file");
            rm_file(destination_file);
            close(dstn_file);
            return RESULT_BAD_WRITE;
        }

        if (write_symb_amount != read_symb_amount) {
            perror("The number of symbols written does't match the number of symbols read.");
            rm_file(destination_file);
            close(dstn_file);
            return RESULT_BAD_WRITE;
        }

        copy_file_size -= read_symb_amount;
        position -= read_symb_amount;

    }

    if (close(cp_file) < 0) {
        perror("Failed close copy file.");
        return RESULT_BAD_CLOSE;
    }

    if (close(dstn_file) < 0) {
        perror("Failed close destination file.");
        return RESULT_BAD_CLOSE;
    }

    free(buf);

    return RESULT_OK;
}


// в параметры командной строки передаются:
// файл-исток      argv[1]
// файл-назначение argv[2]

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        perror("[err] Error message.");
        return RESULT_BAD_ARG;
    }

    // инициализируем структуру
    struct stat sb = {};

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        perror("[err] Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    copy_file(argv[1], argv[2]);

    return RESULT_OK;
}

// TODO: найти способ дописывать в файл, иначе в конечном файле отображается только последний записанный блок