#include <unistd.h>
#include <stdio.h>
#include <signal.h>

//? говорим компилятору, что поведение этой переменной нельзя оптимизировать (volatile)
volatile int g_last_signal;

void sig_handler(int signum) {
    g_last_signal = signum;
}

int main() {

    // сигналы 9 и 18 нельзя перехватить
    //TODO: сделать обработку ошибок
    //TODO: change "signal" to "sigaction"
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    //TODO: какой флаг надо указать, чтобы выводилась дополнительная информация (от кого пришел сигнал)
    signal(SIGINT, sig_handler); // ^C
    signal(SIGQUIT, sig_handler); // ^/
    signal(SIGTSTP, sig_handler); //? ^Z 
    signal(SIGHUP, sig_handler); // обрыв соединения
    signal(SIGTERM, sig_handler); // завершение работы
    
    while(1) {
        pause();
        printf("\tSignal %d came\n", g_last_signal);
    }
    return 0;
}


// man 2 kill
//! man 2 signal 
// man 2 sigaction -> flags: SA_RESTART, SA_SIGINFO
// man 2 pause