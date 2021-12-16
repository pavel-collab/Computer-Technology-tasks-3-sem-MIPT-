//* man lockf flock fcntl

#include <linux/limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/file.h> // flock
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// будем считать, что программа запускается не более 99999 раз
int N_LENGTH = 5;

int main(int argc, char* argv[]) {

    // открывем файл на чтение и запись
    int fd = open("counter.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    /* Initialize the flock structure. */
    struct flock lock = {0};

    //! использовать O_WRLCK и вообще можно использовать просто flock
    //! либо заполнять все поля структуры
    /* Place a read lock on the file. */
    lock.l_type = F_RDLCK;

    //? почему не работает первая ветка условного оператора
    errno = 0;
    int error_state = fcntl(fd, F_SETLK, &lock);
    if (error_state == -1 && errno == EACCES && errno == EAGAIN) {
        perror("fcntl()");
        close(fd);
        return -1;
    }
    

    char buf[N_LENGTH + 1];
    buf[N_LENGTH] = '\0';
    size_t read_sym = read(fd, buf, N_LENGTH);
    if (read_sym == -1) {
        perror("read()");
        close(fd);
        free(buf);
        return -1;
    }

    int count = atoi(buf);
    count += 1;

    // устанавливаем смещение в начало файла
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek()");
        close(fd);
        return -1;
    }

    // записываем новое значение
    if (dprintf(fd, "%d", count) < 0) {
        perror("Failed write to file");
        close(fd);
        return -1;
    }

    /* Release the lock. */
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);

    close(fd);
    return 0;
}