#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void a_out() {
    execlp(
        // path to executable
        "/home/pavel/workfile/ComputerTechnology/11_flock/out",
        //argv
        "out", NULL        
    );
    printf("test -1");
}

void proc_info(const char* procname) {
    printf("[%s]:\n"
           "PID %d,\n"
           "PPID %d,\n"
           "PGID %d,\n"
           "SID %d\n\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

int main() {
    
    for (int i = 0; i < 10; ++i)
    {
        int retFork = -1;
        switch (retFork = fork())
        {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0: /* child */
            a_out();
            break;
        default:
            break;
        }

        if (retFork == 0)
            break;
    }

    // ждем, пока завершатся все дети
    while(wait(NULL) != -1) {
    }

    return 0;
}