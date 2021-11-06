#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

// получаем информацию о процессе
void proc_info(const char* procname) {
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}

//* можно перенаправить поток вывода ошибок в специальный лог файл
int main(int argc, char* argv[]) {
    pid_t child_id = fork();

    if (child_id < 0) {
        perror("fork");
        return -1;
    }

    if (child_id == 0) {
        int file_fd = open("1.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (file_fd < 0) {
            perror("file_fd");
            return -1;
        }
        dup2(file_fd, fileno(stdout));
        close(file_fd);
        proc_info("child");
    }
    else {
        proc_info("parent");
    }

    return 0;
}