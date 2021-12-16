//* gcc -o client client.c -lrt -pthread
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>

size_t TIME_SIZE = sizeof("dd/mm/yy hh:mm:ss (UTC)");

volatile int cought_signum = -1;
// простой обработчик
void handler(int signum) {
    cought_signum = signum;
    dprintf(fileno(stdout), "\tGot signal [%d]\n", cought_signum);
    exit(signum);
}

int main(int argc, char *argv[]) {

    const char* sem_name = "named_sem";

    // докидываем обработчик сигналов
    // --------------------------------------------------------------------------------------------
    struct sigaction int_handler = {0};

    int_handler.sa_handler = handler;
    int_handler.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &int_handler, NULL)) {
        perror("sigaction(SIGTSTP)");
        return -1;
    } // SIGINT -> ^c
    if (sigaction(SIGTSTP, &int_handler, NULL)) {
        perror("sigaction(SIGTSTP)");
        return -1;
    } // SIGTSTP -> ^z
    if (sigaction(SIGTERM, &int_handler, NULL) == -1) {
            perror("sigaction(SIGTSTP)");
            return -1;
    } // завершение работы
    // --------------------------------------------------------------------------------------------
    
    // 
    sem_t* sem = sem_open(sem_name, O_EXCL);
    if (sem == SEM_FAILED) {
        perror("sem_open()");
        return -1;
    }

    const char* shm_name = "/clock";
    int fd;
    char* addr;
    struct stat sb;
    
    fd = shm_open(shm_name, O_RDONLY, 0); /* Open existing object */
    if (fd == -1) {
        perror("shm_open");
        return -1;
    }

    /* Use shared memory object size as length argument for mmap()
    and as number of bytes to write() */
    if (fstat(fd, &sb) == -1) {
        perror("fstat()");
        return -1;
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap()");
        return -1;
    }

    if (close(fd) == -1) { /* 'fd' is no longer needed */
        perror("close()");
        return -1;
    }

    while (1) {
        sem_wait(sem) == -1;
        write(fileno(stdout), addr, sb.st_size);
        printf("\n");
        sem_post(sem) == -1;
        sleep(2);
    }
    
    return 0;
}

/*
Задание:

открывает \clock на чтение и оттображает область общей памяти в свое адресное пространство
в цикле раз в секунду печатает в stdout строку из этой области
по сигналу прекращает работу, не забыв отпустить семафор

sem_wait sem_post
*/