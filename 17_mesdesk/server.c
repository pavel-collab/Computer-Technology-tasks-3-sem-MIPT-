//* gcc -o server -Werror -Wall -Wextra -Wpedantic server.c -lrt

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
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

    // allocate buffer for message
    char* buf = (char*) malloc(q_inf.mq_msgsize);
    if (buf == NULL) {
        perror("malloc");
        return -1;
    }

    while (1) {

        long message = mq_receive(queue, buf, q_inf.mq_msgsize, NULL);
        if (message == -1) {
            perror("mq_receive");
            return -1;
        }

        if (write(STDOUT_FILENO, buf, message) == -1) {
            perror("write");
            return -1;
        }
        write(STDOUT_FILENO, "\n", 1);

        // return
        if (!strncmp(buf, "exit", 4)) {
            break;
        }
    }  

    // remove queue
    mq_unlink(argv[1]);
    // cleanup:
    mq_close(queue);
    free(buf);

    return 0;
}