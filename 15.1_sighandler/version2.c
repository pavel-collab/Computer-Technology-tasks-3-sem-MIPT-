#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sched.h>


//TODO look up the example from the man 2 sigaction
//! there are some warnings

// простой обработчик
void handler(int signum) {
    printf("\tGot signal [%d]\n", signum);
}

// прокачанный обработчик
void sig_handler(int signum, siginfo_t *info, void *ctx) {
    printf("\tGot signal [%d] from proces [%d]\n", signum, info->si_pid);
}

int main() {


    //! для простого обработчика надо заполнять поле sa_handler структуры sigaction
    //! для прокачанного обработчика нужно заполнять поле sa_sigaction
    //! man sigaction

    struct sigaction int_handler = {0};
    // функция заполняет буффуер, на который ссылается указатель, нулями
    // memset(&int_handler, 0, sizeof(int_handler));

    int_handler.sa_handler = handler;
    int_handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &int_handler, NULL); // SIGINT -> ^c

    //TODO дописать обработку остальных сигналов
    struct sigaction term_handler = {0};
    // memset(&term_handler, 0, sizeof(term_handler));

    term_handler.sa_sigaction = sig_handler;
    term_handler.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGTSTP, &term_handler, NULL); // SIGTSTP -> ^z

    while(1) {
        sched_yield();
    }

    return 0;
}