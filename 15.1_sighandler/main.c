#include <unistd.h>
#include <stdio.h>
#include <signal.h>

// поведение этой переменной нельзя оптимизировать
volatile int g_last_signal;

void sig_handler(int signum) {
    g_last_signal = signum;
}

int main() {

    // чтобы убить прогу выполнить в параллельном терминале kill -9 <pid>
    // чтобы посмотреть pid процесса в параллельном терминале ps ux

    //! сигналы 9 и 18 нельзя перехватить
    if (signal(SIGINT, sig_handler) == SIG_ERR){
        perror("signal(SIGINT)");
        return -1;
    } // ^c
    if (signal(SIGQUIT, sig_handler)) {
        perror("signal(SIGQUIT)");
        return -1;
    } // ^/
    if (signal(SIGTSTP, sig_handler)) {
        perror("signal(SIGTSTP)");
        return -1;
    } // ^Z 
    if (signal(SIGHUP, sig_handler)) {
        perror("signal(SIGHUP)");
        return -1;
    } // обрыв соединения
    if (signal(SIGTERM, sig_handler)) {
        perror("signal(SIGTERM)");
        return -1;
    } // завершение работы
    
    printf("to discover process pid, in another console use:\n \tps ux\n");
    printf("to kill programm, in another console use:\n \tkill -9 <pid>\n\n");
    while(1) {
        pause();
        printf("\tSignal %d came\n", g_last_signal);
    }
    return 0;
}