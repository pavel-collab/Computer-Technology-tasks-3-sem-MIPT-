//* man lockf flock fcntl

#include <linux/limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/file.h> // flock
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// будем считать, что программа запускается не более 1000 раз
int N_LENGTH = 4;

// функция для получения полного пути к файлу
char* get_path(char* exepath) {
    char arg1[20];

    sprintf(arg1, "/proc/%d/exe", getpid());
    readlink(arg1, exepath, 1024);
    return exepath;
}

void status(struct flock *lock) {

    printf("Status: ");

    switch(lock->l_type){

        case F_UNLCK:
            printf("F_UNLCK (unlocking)\n");
            break;

        case F_RDLCK:
            printf("F_RDLCK (pid: %d) (read lock)\n", lock->l_pid);
            break;

        case F_WRLCK:
            printf("F_WRLCK (pid: %d) (write lock)\n", lock->l_pid);
            break;

        default : break;

    }

}

int main(int argc, char* argv[]) {
    
    char exepath[PATH_MAX + 1] = {0};
    char* full_programm_name = get_path(exepath);
    // printf("full_programm_name = %s\n", full_programm_name);
    // printf("%s\n", argv[0]);

    // открывем файл на чтение и запись
    int fd = open("counter.txt", O_RDWR);
    if (fd == -1) {
        perror("open()");
        return -1;
    }

    struct flock lock;
    /* Initialize the flock structure. */
    memset(&lock, 0, sizeof(lock));

    lock.l_type = F_RDLCK;
    /* Place a read lock on the file. */
    if (fcntl(fd, F_SETLKW, &lock) == -1 && (errno == EACCES || errno == EAGAIN)) {
        status(&lock);
    }
    else if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl()");
        close(fd);
        return -1;
    }

    char* buf = (char*) calloc(N_LENGTH, sizeof(char));

    size_t read_sym = read(fd, buf, N_LENGTH);
    if (read_sym == -1) {
        perror("read()");
        close(fd);
        free(buf);
        return -1;
    }

    int count = atoi(buf);
    count+=1;

    // устанавливаем смещение в начало файла
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek()");
        close(fd);
        free(buf);
        return -1;
    }

    // записываем новое значение
    if (dprintf(fd, "%d", count) < 0) {
        perror("Failed write to file");
        close(fd);
        free(buf);
        return -1;
    }

    /* Release the lock. */
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    status(&lock);

    close(fd);
    free(buf);
    return 0;
}