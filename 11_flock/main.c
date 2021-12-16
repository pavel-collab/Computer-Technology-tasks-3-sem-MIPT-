//* man lockf flock fcntl

#include <linux/limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/file.h> // flock
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// будем считать, что программа запускается не более 99999999999 раз
int N_LENGTH = sizeof("99999999999");

int main(int argc, char* argv[]) {

    // открывем файл на чтение и запись
    int fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    char buf[N_LENGTH];
    memset(buf, 0, sizeof(buf));
    int counter = 1;

    // Locking file for reading and writing
    lockf(fd, F_LOCK, 0);

    long int bytes_read = read(fd, buf, N_LENGTH);
    if(bytes_read < 0) {
        perror("read");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        return -1;
    }

    sleep(5);

    char number[bytes_read > 0 ? (bytes_read + 1): 2];
    memset(number, 0, sizeof(number));

    counter += atoi(buf);

    printf("The program was executed %d times\n", counter);
    sprintf(number, "%d\n", counter);

    if(pwrite(fd, number, sizeof(number), 0) < 0) {
        perror("write");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        return -1;
    }

    // Unlocking and closing file after work is done
    lockf(fd, F_ULOCK, 0);

    close(fd);
    return 0;
}