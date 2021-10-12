//* Compile with gcc -Wall -Wextra -o out main.c

#define _XOPEN_SOURCE_EXTENDED
// в каталоге .git хранится много файлов, при тестировании таска они засоряют вывод
// чтобы выводить содердимое .git раскомментируйте define и пересоберите таск

// #define DOT_GIT_VEIW

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
// --------------------------------------------------------------------------------------------
#include "../CT_tasks.h"

int main(int argc, char* argv[]) {

    char* dir_name = ".";
    if (argc == 2) {
        dir_name = argv[1];
    }
    
    int sys_dir_fd = open(dir_name, O_RDONLY);
    if (sys_dir_fd < 0) {
        perror("Failed for open file");
        return RESULT_OPEN_FAILED;
    }

    RunDir(sys_dir_fd, 0); 
    close(sys_dir_fd);  
    return 0;
}