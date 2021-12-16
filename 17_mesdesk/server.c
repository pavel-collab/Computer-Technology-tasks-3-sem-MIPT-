//* gcc -o server -Werror -Wall -Wextra -Wpedantic server.c -lrt

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

mqd_t queue;
char* queue_name;

// простой обработчик
void handler(int signum) {
    // remove queue
    mq_unlink(queue_name);
    // cleanup:
    mq_close(queue);
    exit(signum);
}

void PrintInfo(const char* q_name, struct mq_attr* q_inf) {
    printf("queue name:                %s\n", q_name);
    printf("queue flags :              %ld\n", q_inf->mq_flags);
    printf("Max message amount :       %ld\n", q_inf->mq_maxmsg);
    printf("Max message size (bytes) : %ld (%.1f pages)\n", q_inf->mq_msgsize, round(q_inf->mq_msgsize/4096));
    printf("Current message amount :   %ld\n", q_inf->mq_curmsgs);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage %s /queue_name\n", argv[0]);
        return 1;
    }
    queue_name = argv[1];

    // докидываем обработчик сигналов
    // --------------------------------------------------------------------------------------------
    struct sigaction int_handler = {0};
    // функция заполняет буффуер, на который ссылается указатель, нулями

    int_handler.sa_handler = handler;
    int_handler.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &int_handler, NULL)) {
        perror("sigaction(SIGTSTP)");
        return -1;
    } // SIGINT -> ^c
    if (sigaction(SIGTERM, &int_handler, NULL) == -1) {
            perror("sigaction(SIGTSTP)");
            return -1;
    } // завершение работы
    // --------------------------------------------------------------------------------------------

    // quque creating
    queue = mq_open(queue_name, O_RDWR | O_CREAT, 0622, NULL);
    if (queue == (mqd_t) -1) {
        perror("mq_open");
        return 1;
    }

    // get info
    struct mq_attr q_inf;
    mq_getattr(queue, &q_inf);

    long mq_msgsize = q_inf.mq_msgsize;
    char buf[mq_msgsize + 1];

    while (1) {

        size_t message = mq_receive(queue, buf, mq_msgsize, NULL);
        if (message == (size_t) -1) {
            perror("mq_receive");
            break;
        }

        buf[message] = '\n';
        if (write(STDOUT_FILENO, buf, message + 1) == -1) {
            perror("write");
            break;
        }
    }  

    return 0;
}