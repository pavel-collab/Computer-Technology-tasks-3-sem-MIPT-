#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sched.h>

// #define SIMPLE_HANDLER

// нельзя оптимизировать использование этой переменной
volatile int cought_signum;
volatile int proc_pid;

//! пихать printf в обработчик плохо, потому что printf не является асинхронно-безопасной
// простой обработчик
void handler(int signum) {
    // printf("\tGot signal [%d]\n", signum);
    cought_signum = signum;
}

// прокачанный обработчик
void sig_handler(int signum, siginfo_t *info, void *ctx) {
    // printf("\tGot signal [%d] from proces [%d]\n", signum, info->si_pid);
    cought_signum = signum;
    proc_pid = info->si_pid;
}

int main() {


    //! для простого обработчика надо заполнять поле sa_handler структуры sigaction
    //! для прокачанного обработчика нужно заполнять поле sa_sigaction
    //! man sigaction

    #ifdef SIMPLE_HANDLER
        struct sigaction int_handler = {0};
        // функция заполняет буффуер, на который ссылается указатель, нулями

        int_handler.sa_handler = handler;
        int_handler.sa_flags = SA_RESTART;
        sigaction(SIGINT, &int_handler, NULL); // SIGINT -> ^c
    #endif

    //TODO дописать обработку остальных сигналов
    struct sigaction term_handler = {0};

    term_handler.sa_sigaction = sig_handler;
    term_handler.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGTSTP, &term_handler, NULL); // SIGTSTP -> ^z

    while(1) {
        pause(); // wait until signum cought and processed
        // printf("\tGot signal [%d]\n", cought_signum);
        printf("\tGot signal [%d] from proces [%d]\n", cought_signum, proc_pid);
    }

    return 0;
}

// обработка сигналов
//* SIGTERM (15 -- завершение работы), SIGSTOP (^z) -- корректно завершить работу
//* SIGINT (^c) -- возможно спросить: "вы уверены?"
//? SIGHUB (23 -- обрыв соединения) -- перечитать файл с настройками
//* SIGCHILD (17 -- завершение дочернего процесса) -- прочитать код возврата дочернего процесса
//* SIGSEGV (11 -- что-то плохо с памятью) -- попытаться сохранить важные данные