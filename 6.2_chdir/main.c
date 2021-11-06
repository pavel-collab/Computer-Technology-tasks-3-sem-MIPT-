//* Compile with gcc -Wall -Wextra -o out main.c
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
// --------------------------------------------------------------------------------------------
#include "../flib.h"

unsigned int BUF_SIZE = 32;

int main(int argc, char* argv[]) {

    char* dir_name = ".";
    if (argc == 2) {
        dir_name = argv[1];
    }

    if (chdir(dir_name) < 0) {
        perror("It's not possible to change directory now");
        return RESULT_ERR;
    }

    DIR *dir_fd = opendir(dir_name);
    
    if (!dir_fd) {
        perror("Bad directory ");
        return RESULT_ERR;
    }

    struct dirent* entry;
    printf("dir: %s\n\n", dir_name);

    while ((entry = readdir(dir_fd)) != NULL) {

        char entry_type = dtype_char(entry->d_type);

        struct stat sb;
        assert((lstat(entry->d_name, &sb)) == 0);

        char buf[BUF_SIZE];
        get_access(sb.st_mode, buf);
        printf("[%s] ", buf);

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        printf("%c %s\n", entry_type, entry->d_name);
    }

    closedir(dir_fd);
    return 0;
}