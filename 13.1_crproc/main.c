#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// получаем информацию о процессе
void proc_info(const char* procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
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
        printf("Child PID is %ld\n", (long) getpid());
        if (argc == 1)
            pause();            /* Wait for signals */
        _exit(atoi(argv[1]));   // завердиться с кодом, который передали в аргументе 1

    } 
    else {                      /* Code executed by parent */

        do {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            // waitpid -- процесс родитель дожидается ИЗМЕНЕНИЯ статуса процесса потомка
            // далее, в зависимости от статуса процесса потомка, программа выводит сообщение об это статусе
            if (WIFEXITED(wstatus)) {
                printf("exited, status=%d\n", WEXITSTATUS(wstatus));
            } else if (WIFSIGNALED(wstatus)) {
                printf("killed by signal %d\n", WTERMSIG(wstatus));
            } else if (WIFSTOPPED(wstatus)) {
                printf("stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                printf("continued\n");
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

        exit(EXIT_SUCCESS);
    }
}
