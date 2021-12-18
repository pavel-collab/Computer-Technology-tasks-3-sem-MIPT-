#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "header.h"

double Laplass(double x) {
    return 1 / (sqrt(2*M_PI) * exp(- x*x / 2));
}

void* ThrSympson(void* arg) {

    arg_t* args = (arg_t*) arg;

    double (*f)(double) = args->function;
    double x_1 = args->int_begin;
    double x_2 = args->int_end;

    int N = args->point_amount;

    double local_sum = 0;
    double d = (x_2 - x_1) / N;

    for (unsigned i = 0; i < N; i++) {
        double a = x_1 + i * d;
        double b = x_1 + (i + 1) * d;
        local_sum += (d / 6) * (f(a) + f(b) + 4 * f((a + b) / 2));
    }

    pthread_mutex_lock(&args->g_mutex);
    *(args->sum) += local_sum;
    pthread_mutex_unlock(&args->g_mutex);

    return NULL;
}