//* gcc -o out atomic.c -lpthread
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    // _Atomic unsigned long long
    atomic_ullong counter;
    int iterations;
} work_area_t;

void* ThrFunction(void* arg) {
    work_area_t* w = arg;
    for (int i = 0; i < w->iterations; i++) {
        atomic_fetch_add(&w->counter, 1); // атомарное суммирование
    }  

    return NULL;
}

int main(int argc, char * argv[]) {
    // атомарное суммрование в 2х потоках
    work_area_t data;

    // присваиваем счетчику значение 0
    atomic_store(&data.counter, 0);
    data.iterations = (argc > 1) ? atoi(argv[1]) : 1000; // можем задать число итераций в argv[1]
    
    pthread_t secondary_thread_id;
    if(errno = pthread_create(&secondary_thread_id, NULL, ThrFunction, &data)) { //pthread_create returns error code
        perror("pthread_create");
        return 1;
    }

    for (int i = 0; i < data.iterations; i++) {
        atomic_fetch_add(&data.counter, 1);
    }

    pthread_join(secondary_thread_id, NULL);
    // atomic_load -- atomic read
    printf("counter = %llu\n", atomic_load(&data.counter));

    return 0;
}