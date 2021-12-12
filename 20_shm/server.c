//* gcc -o server server.c -lrt -pthread
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
#include <time.h>
#include <string.h>

// #define DEBUG

size_t TIME_SIZE = sizeof("dd/mm/yy hh:mm:ss (UTC)");

volatile int cought_signum = -1;
// простой обработчик
void handler(int signum) {
    cought_signum = signum;
    dprintf(fileno(stdout), "\tGot signal [%d]\n", cought_signum);
    //! не чистим ресурсы!!!
    // exit(signum);
}

int get_cur_time(char* time_str) {
    time_t cur_time;
    struct tm timeinfo;
    if (time(&cur_time) == (time_t) -1) {
        perror("time");
        return -1;
    }

    // localtime е является потокобезопасной функцией
    // используем вместо нее localtime_r
    if (localtime_r(&cur_time, &timeinfo) == NULL) {
        perror("localtime_r()");
        return -1;
    }

    if (strftime(time_str, TIME_SIZE, "%x %X (UTC)", &timeinfo) == 0) {
        perror("strftime");
        return -1;
    }

    return 0;
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

    // реализуем неименованный семафор
    // поскольку семафор используется не для родственных процессов
    // он должен располагаться в области разделяемой памяти
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
    if (sem_init(sem, 1, 1) == -1) {
        perror("sem_init()");
        return -1;
    }
    // ===========================================================================================

    const char* shm_name = "/clock";

    int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0666); /* Open existing object */
    if (fd == -1) {
        perror("shm_open");
        return -1;
    }

    // выделяем на наши нужды 1 страницу
    int len = 4096;
    if (ftruncate(fd, len) == -1) { /* Resize object to hold string */
        perror("ftruncate()");
        return -1;
    }
    
    #ifdef DEBUG
        printf("Resized to %ld bytes\n", (long) len);
    #endif

    void* addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap()");
        return -1;
    }

    if (close(fd) == -1) { /* 'fd' is no longer needed */
        perror("close()");
        return -1;
    }

    char* time_str = (char*) calloc(TIME_SIZE, sizeof(char));
    while (cought_signum == -1) {

        if (get_cur_time(time_str) == -1) {
            perror("get_cur_time");
            break;
        }

        if (sem_wait(sem) == -1) {
            perror("sem_wait()");
            break;
        }

        memcpy(addr, time_str, strlen(time_str)); /* Copy string to shared memory */
        #ifdef DEBUG
            printf("%s\n", time_str);
            printf("copying %ld bytes\n", (long) strlen(time_str));
        #endif

        if (sem_post(sem) == -1) {
            perror("sem_post()");
            break;
        }
        sleep(2);
    }

    #ifdef DEBUG
        printf("freeing resources\n");
    #endif
    // удаляем область разделяемой памяти
    shm_unlink(shm_name);
    // удаляем семафор
    sem_destroy(sem);
    // удаляем область разделяемой памяти
    shm_unlink(sem_shm);

    free(time_str);
    return 0;
}

// посмотреть на созданную область разделяемой памяти:
// ls -l /dev/shm 

/* 
Задание:

server -- создает облсть разделяемой памяти (shm_open + ftruncate)
отображает ее в свое адресное пространство (mmap)
и инициализирует область (sem_init)
раз в секунду пишет в нее строку с текущим временем (localtime + strftime)
прекращает работу по сигналу SIGINT/SIGTERM
удаляетобласть общей памяти
*/