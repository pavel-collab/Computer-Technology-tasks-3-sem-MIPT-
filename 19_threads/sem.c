//* gcc -o out mutex.c lib.c -lpthread -lm

#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage %s x1 x2", argv[0]);
        return -1;
    }

    // amount of using threads
    int thread_amount = 4;

    // integration limits
    double a = atof(argv[1]);
    double b = atof(argv[2]);

    
    return 0;
}