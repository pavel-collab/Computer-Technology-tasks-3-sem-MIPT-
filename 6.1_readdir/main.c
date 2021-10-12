//* Compile with gcc -Wall -Wextra -o out main.c
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
// --------------------------------------------------------------------------------------------
#include "../CT_tasks.h"

unsigned int BUF_SIZE = 32;

int main() {
    DIR *dir_fd = opendir(".");

    if (!dir_fd) {
        perror("Can't open current directory");
        return RESULT_OPEN_FAILED;
    }

    struct dirent* entry;

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
    return RESULT_OK;
}