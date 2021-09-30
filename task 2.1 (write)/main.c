//* Compile with gcc -Wall -Wextra -o out main.c

#include <stdint.h> // uint8_t
#include <string.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../enum.h"

// size_t  -- беззнаковый
// ssize_t -- знаковый
// unit8_t -- unsigned 8-bit

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

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        //puts("Expect more arguments.");
        return RESULT_BAD_ARG;
    }

    // r/w for user, read only for group and for others
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        perror("Failed for open file for writing");
        return RESULT_OPEN_FAILED;
    }

    if (writeall(fd, argv[2], strlen(argv[2])) < 0) {
        perror("Failed write to file");
        close(fd);
        return RESULT_BAD_WRITE;
    }

    if (close(fd) < 0) {
        perror("Failed close file.");
        return RESULT_BAD_CLOSE;
    }

    //return RESULT_OK;
}