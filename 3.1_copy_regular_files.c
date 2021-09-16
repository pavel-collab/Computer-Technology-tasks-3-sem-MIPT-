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
    RESULT_BAD_CLOSE
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

// в параметры командной строки передаются:
// файл-исток      argv[1]
// файл-назначение argv[2]

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        puts("Error message.");
        return RESULT_BAD_ARG;
    }

    // инициализируем структуру
    struct stat sb = {};

    if (lstat(argv[2], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        puts("Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    int copy_file        = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int destination_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (copy_file < 0) {
        perror("Failed for open copy file for writing");
        return RESULT_OPEN_FAILED;
    }

    if (destination_file < 0) {
        perror("Failed for open destination file for writing");
        return RESULT_OPEN_FAILED;
    }

    char str[256] = "";

    if (read(copy_file, &str, 256) < 0) {
        perror("Failed read from the file");
        close(copy_file);
        return RESULT_BAD_READ;
    }

    if (writeall(destination_file, &str, 256) < 0) {
        perror("Failed write to file");
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

    return RESULT_OK;
}

//TODO: заменить параметры open() -- только чтение или только запись
//TODO: man 2 read -> read BUGS

//ssize_t read(int fd, void *buf, size_t count);

// open the first file, open the second file -> 
// read from the first file                  -> 
// write to the second file                  ->
// sclose the first file, close the second file