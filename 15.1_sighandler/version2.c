#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

//TODO look up the example from the man 2 sigaction
//! there are some warnings

void handler(int signum) {
    printf("\tGot signal [%d]\n", signum);
}

void sig_handler(int signum, siginfo_t *info, void *ctx) {
    printf("\tGot signal [%d] from poces [%d]\n", signum, info->si_pid);
}

int main() {

    struct sigaction int_handler;
    // функция заполняет буффуер, на который ссылается указатель, нулями
    memset(&int_handler, 0, sizeof(int_handler));

    int_handler.sa_handler = handler;
    int_handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &int_handler, NULL);

    //TODO дописать обработку остальных сигналов
    struct sigaction term_handler;
    memset(&term_handler, 0, sizeof(term_handler));

    term_handler.sa_handler = sig_handler;
    term_handler.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGTERM, &term_handler, NULL);

    while(1) {
        sched_yield(); // man 3 sched_yield
    }

    return 0;
}