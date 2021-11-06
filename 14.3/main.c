#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

//! look up man 2 pipe
//! look up the example from the man 2 pipe

//TODO: fdopen писать в pipe с помощью, напрмер, fprintf
//TODO: dlopen() dlclose() -> integrate problem

int main(int argc, char* argv[]) {

    int pips_fds[2];
    if (pipe(pips_fds) < 0) {
        perror("failed to create pipe");
        return -1;
    }

    pid_t child_id = fork();

    if (child_id < 0) {
        perror("fork");
        return -1;
    }

    if (child_id == 0) {
        // вызываем last в дочернем процессе, чтобы он завершился раньше, чем родительский
        // тогда не произойдет ситуации, когда wc -l закончил работу раньше, чем least

        // close reading end of the pipe
        close(pips_fds[0]);
        // redirect stdout to pipe
        dup2(pips_fds[1], fileno(stdout));
        close(pips_fds[1]); // исходный дескриптор нам больше не нужен

        execlp("last", "last", NULL);
        perror("failed to exec last");
    }

    // close writing end of the pipe
    close(pips_fds[1]);

    dup2(pips_fds[0], fileno(stdin));
    close(pips_fds[0]);

    execlp("wc", "wc", "-l", NULL);
    perror("failed to exec wc -l");


    return 0;
}