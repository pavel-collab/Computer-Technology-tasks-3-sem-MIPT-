//* gcc -o out mutex.c lib.c -lpthread -lm

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage %s x1 x2\n", argv[0]);
        return -1;
    }

    // amount of using threads
    int thread_amount = 4;

    // integration limits
    double a = atof(argv[1]);
    double b = atof(argv[2]);

    // variable to the final result
    volatile double main_sum = 0;

    // array of structures for threads arguments
    arg_t thread_args[thread_amount];

    // mutex init
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // в цикле задаем аргументы каждому процессу
    for (int i = 0; i < thread_amount; i++) {
        thread_args[i].g_mutex = mutex;
        thread_args[i].sum = &main_sum;
        thread_args[i].function = Laplass;
        thread_args[i].int_begin = a + ((b - a) / thread_amount) * i;
        thread_args[i].int_end = a + ((b - a) / thread_amount) * (i + 1);
        thread_args[i].point_amount = 5000000;
    }

    // threads array
    pthread_t thread_id[thread_amount]; 

    // create threads
    for (int i = 0; i < thread_amount; ++i) {
        if (errno = pthread_create(&thread_id[i], NULL, ThrSympson, &thread_args[i])) {
            //! прибивать потоки и чистить ресурсы в случае аварийной остановки программы
            perror("pthread_create");
            return 1;
        }
    }

    // wait for a thread [thread_id]
    for (int i = 0; i < thread_amount; ++i) {
        pthread_join(thread_id[i], NULL);
    }

    printf("final sum %f\n", main_sum);
    return 0;
}