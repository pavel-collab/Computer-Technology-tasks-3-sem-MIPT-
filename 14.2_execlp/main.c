#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    
    // execlp(
    //     // path to executable
    //     "grep",
    //     //argv
    //     "grep", "model name", "/proc/cpuinfo", NULL        
    // );
    // printf("test -1");

    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    
    // printf("test 0");
    // fflush(stdout);
    // execlp(
    //     // path to executable
    //     "echo",
    //     //argv
    //     "echo", "test 1", NULL        
    // );
    // perror("failed to exec grep");

    //! пример того, как 2 потока пишут в 1 файл (в данном случае stdout)
    putchar('a');
    // ffkush() перед форком делает выгрузку буффера
    // fflush(stdout); //! что будет, если закоментить эту строчку
    if (fork() == 0) {
        putchar('b');
        return 0;
    }
    putchar('c');

    return 0;
}