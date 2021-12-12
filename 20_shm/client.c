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
    //! не чистим ресурсы!!!
    exit(signum);
}

int main(int argc, char *argv[]) {

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

    // получаем неименованный семафор
    // ===========================================================================================
    const char* sem_shm = "/sem_shm";
    sem_t *sem;
    int sem_fd;

    sem_fd = shm_open(sem_shm, O_CREAT | O_RDWR, 0666);
    if (sem_fd == -1) {
        perror("shm_open(sem_fd)");
        return -1;
    }

    ftruncate(sem_fd, sizeof(sem_t));
    sem = (sem_t*) mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, sem_fd, 0);
    // ===========================================================================================
    
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
        if (sem_wait(sem) == -1) {
            perror("sem_wait()");
            break;
        }
        write(fileno(stdout), addr, sb.st_size);
        printf("\n");
        if (sem_post(sem) == -1) {
            perror("sem_post()");
            break;
        }
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