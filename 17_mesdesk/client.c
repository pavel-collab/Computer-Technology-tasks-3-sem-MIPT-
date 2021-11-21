//* gcc -o client -Werror -Wall -Wextra -Wpedantic client.c -lrt

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <string.h>

// client
int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage %s /queue_name 'message'\n", argv[0]);
        return 1;
    }

    // quque creating
    mqd_t queue = mq_open(argv[1], O_WRONLY, NULL);
    if (queue == (mqd_t) -1) {
        perror("mq_open");
        return 1;
    }
    // get info
    struct mq_attr q_inf;
    if (mq_getattr(queue, &q_inf) == -1) {
        perror("mq_getattr");
    }

    // send message
    if (mq_send(queue, argv[2], strlen(argv[2]), 0) == -1) {
        perror("mq_send");
    }

    // cleanup:
    mq_close(queue);

    return 0;
}