//* Compile with gcc -Wall -Wextra -o out main.c
// go to man 2 pwrite
#define _XOPEN_SOURCE 500

#include <stdint.h> // uint8_t
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <assert.h>
// --------------------------------------------------------------------------------------------
#include "../enum.h"

const unsigned int MAX_LEN = 5;

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
int rm_file(const char* filename) {

    if (unlink(filename) == -1) {
        perror("It's not possible to remove this file");
        return RESULT_ERR;
    }
    printf("[+] Successful removing: %s", filename);
    return RESULT_OK;
}

// функция проверяет права пользователя
int check_user_access(const char* file_name, const char access) {

    struct stat sb = {};

    if (lstat(file_name, &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    switch (access) {
        case 'r': return (sb.st_mode & S_IRUSR) ? 1 : 0;
        case 'w': return (sb.st_mode & S_IWUSR) ? 1 : 0;
        case 'x': return (sb.st_mode & S_IXUSR) ? 1 : 0;
        default:  return -1;
    }
}

int copy_file(unsigned cp_file, unsigned dstn_file, const char* destination_file, struct stat *sb) {

    // аллоцирем буфер для чтения
    char* buf = (char*) calloc(MAX_LEN, sizeof(char));
    assert(buf != NULL);

    long long copy_file_size = (long long) sb->st_size;

    // копируем информацию
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

        ssize_t write_symb_amount = writeall(dstn_file, buf, read_symb_amount);

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

    free(buf);

    return RESULT_OK;
}


// в параметры командной строки передаются:
// файл-исток      argv[1]
// файл-назначение argv[2]

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        puts("[err] Error message.");
        return RESULT_BAD_ARG;
    }

    // ===========================================================================================

    struct stat sb;

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        puts("[err] Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    // ===========================================================================================

    int cp_file   = open(argv[1], O_RDONLY);
    if (cp_file < 0) {
        perror("Failed for open copy file for writing");
        rm_file(argv[2]);
        return RESULT_OPEN_FAILED;
    }

    int dstn_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
    if (dstn_file < 0) {
        perror("Failed for open destination file for writing");
        rm_file(argv[2]);
        return RESULT_OPEN_FAILED;
    }

    // ===========================================================================================
    
    // проверяем право на чтение файла
    if (check_user_access(argv[1], 'r') != 1) {
        fprintf(stderr, "Usage: %s filename\n", argv[2]);
        perror("This file can't be read");
        return RESULT_BAD_READ;
    }

    // проверяем право на запись в файл
    if (check_user_access(argv[2], 'w') != 1) {
        fprintf(stderr, "Usage: %s filename\n", argv[2]);
        perror("This file can't be written in");
        return RESULT_BAD_WRITE;
    }

    // ===========================================================================================
    
    copy_file(cp_file, dstn_file, argv[2], &sb);

    // ===========================================================================================

    if (close(cp_file) < 0) {
        perror("Failed close copy file.");
        return RESULT_BAD_CLOSE;
    }

    if (close(dstn_file) < 0) {
        perror("Failed close destination file.");
        return RESULT_BAD_CLOSE;
    }

    return RESULT_OK;
}
