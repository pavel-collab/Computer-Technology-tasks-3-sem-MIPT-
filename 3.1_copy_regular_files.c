//Compile with: gcc -Wall -Wextra -o 3.1_copy 3.1_copy_regular_files.c

#include <stdint.h> // uint8_t

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

enum {
    RESULT_OK = 0,
    RESULT_BAD_ARG,
    RESULT_BAD_FILE_TYPE,
    RESULT_OPEN_FAILED, 
    RESULT_BAD_READ,
    RESULT_BAD_WRITE,
    RESULT_BAD_CLOSE,
    RESULT_BAD_FILE_DELETE
};

ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = buf;

    while (bytes_written < count) {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);

        if (res < 0) {
            return res;
        }

        bytes_written += (size_t) res;
    }
    return (ssize_t) bytes_written;
}

// the function of the removing fire from the dir
int rm_file(char* filename) {

    // конкатенация строк
    const char *rm  = "rm ";

    char command[512];
    snprintf(command, sizeof command, "%s%s", rm, filename);

    system(command);
    printf("[+] Successful removing: %s", filename);

    return 0;
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

    // инициализируем структуру
    struct stat sb = {};

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        puts("[err] Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    int copy_file        = open(argv[1], O_RDONLY);
    int destination_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (copy_file < 0) {
        perror("Failed for open copy file for writing");
        rm_file(argv[2]);
        return RESULT_OPEN_FAILED;
    }

    if (destination_file < 0) {
        perror("Failed for open destination file for writing");
        rm_file(argv[2]);
        return RESULT_OPEN_FAILED;
    }

    // узнаем размер файла в байтах
    long long copy_file_size = (long long) sb.st_size;

    char* buf = (char*) calloc(copy_file_size, sizeof(char));

    if (read(copy_file, buf, copy_file_size) < 0) {
        perror("Failed read from the file");
        rm_file(argv[2]);
        close(copy_file);
        return RESULT_BAD_READ;
    }

    if (writeall(destination_file, buf, copy_file_size) < 0) {
        perror("Failed write to file");
        rm_file(argv[2]);
        close(destination_file);
        return RESULT_BAD_WRITE;
    }

    if (close(copy_file) < 0) {
        perror("Failed close copy file.");
        return RESULT_BAD_CLOSE;
    }

    if (close(destination_file) < 0) {
        perror("Failed close destination file.");
        return RESULT_BAD_CLOSE;
    }

    free(buf);

    return RESULT_OK;
}

//TODO: man 2 read -> read BUGS

//ssize_t read(int fd, void *buf, size_t count);

// open the first file, open the second file -> 
// read from the first file                  -> 
// write to the second file                  ->
// sclose the first file, close the second file