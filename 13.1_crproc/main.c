#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// получаем информацию о процессе
void proc_info(const char* procname) {
    printf("[%s]:\n"
           "PID %d,\n"
           "PPID %d,\n"
           "PGID %d,\n"
           "SID %d\n\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main(int argc, char *argv[]) {
    pid_t cpid, w;
    int wstatus;

    // создаем дочерний процесс
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {            /* Code executed by child */
        proc_info("Child");
        if (argc == 1)
            pause();            /* Wait for signals */
        _exit(atoi(argv[1]));   // завершится с кодом, который передали в аргументе 1

    } 
    else {                      /* Code executed by parent */
        proc_info("Parent");
        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            // waitpid -- процесс родитель дожидается ИЗМЕНЕНИЯ статуса процесса потомка
            // далее, в зависимости от статуса процесса потомка, программа выводит сообщение об это статусе
            // --------------------------------------------------------------------------------------------
            if (WIFEXITED(wstatus)) {
                printf("the child process has been completed\nstatus = %d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                printf("the child process has been killed by signal %d\n", WTERMSIG(wstatus));
            } else if (WIFSTOPPED(wstatus)) {
                printf("the child process has been stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                printf("continued\n");
            }

            // core dump
            if (WCOREDUMP(wstatus)) {
                printf("child proc has cause a core dump\n");
            }

        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
        // крутимся в цикле, пока дочерний процесс не завершиться (успешно или по сигналу)

        exit(EXIT_SUCCESS);
    }
}

/*

WIFEXITED(wstatus)   -- не равно нулю, если дочерний процесс успешно завершился

WIFSIGNALED(wstatus) -- возвращает истинное значение, если дочерний процесс завершился из-за необработанного сигнала

WIFSTOPPED(wstatus)  -- возвращает истинное значение, если дочерний процесс, 
                        из-за которого функция вернула управление, в настоящий момент остановлен; 
                        это возможно, только если использовался флаг WUNTRACED

*/

// ./out 0 -> the child process has been completed 
//            status = 0

// ./out -> ^Z -> ps -> kill -18 <cpid> -> the child process has been stopped by signal 20 
//                                         continued
//                                         child proc has cause a core dump

// ./out -> ^Z -> ps -> kill -11 <cpid> -> the child process has been killed by signal 11
//                                         child proc has cause a core dump