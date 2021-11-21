#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

void PrintInfo(char* q_name, struct mq_attr* q_inf) {
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

    // quque creating
    mqd_t queue = mq_open(argv[1], O_RDWR | O_CREAT, 0600, NULL);
    if (queue == (mqd_t) -1) {
        perror("mq_open");
        return 1;
    }

    // get info
    struct mq_attr q_inf;
    if (mq_getattr(queue, &q_inf) == -1) {
        perror("mq_getattr");
    }

    PrintInfo(argv[1], &q_inf);

    // send test message
    if (mq_send(queue, "hello", strlen("hello"), 0) == -1) {
        perror("mq_send");
    }

    if (mq_getattr(queue, &q_inf) == -1) {
        perror("mq_getattr");
    }

    PrintInfo(argv[1], &q_inf);

    char* buf = (char*) malloc(q_inf.mq_msgsize);
    if (mq_receive(queue, buf, q_inf.mq_msgsize, NULL) == -1) {
        perror("mq_receive");
        free(buf);
        return -1;
    }

    if (mq_getattr(queue, &q_inf) == -1) {
        perror("mq_getattr");
    }
    PrintInfo(argv[1], &q_inf);

    // поскольку очередь -- глобальный объект, ее закрытие и даже завершение программы 
    // не приводит к удалению
    mq_unlink(argv[1]);
    // cleanup:
    mq_close(queue);
    free(buf);
    return 0;
}
//? mq_timedsend
//? mq_getattr
//? использовать для записи sys call write

//TODO: в 17й написать читателя
//TODO: 17я задача -- читатель и писатель. Сервер создает очередь и читает ее в вечном цикле (mq_receive)
//TODO: и печатает то, что он получил (write to stdout)