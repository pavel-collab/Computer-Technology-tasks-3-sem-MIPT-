#ifndef HEADER_H_
#define HEADER_H_

typedef struct {
    pthread_mutex_t g_mutex;
    volatile double* sum;

    double (*function)(double);
    double int_begin;
    double int_end;

    int point_amount;
} arg_t;

double Laplass(double x);
void* ThrSympson(void* arg);

#endif