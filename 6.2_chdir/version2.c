//* Compile with gcc -Wall -Wextra -o version2 version2.c
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
// --------------------------------------------------------------------------------------------
#include "../flib.h"

const unsigned BUF_SIZE = 9;

int main(int argc, char* argv[]) {

    char* dir_name = ".";
    if (argc == 2) {
        dir_name = argv[1];
    }

    DIR* dir_fd = opendir(dir_name);

    // возвращает системный файловый дискриптор по C-шному файловому дискриптору
    int sys_dir_fd = dirfd(dir_fd);
    
    if (!dir_fd) {
        perror("Bad dir file descriptor");
        return RESULT_ERR;
    }

    struct dirent* entry;
    printf("dir: %s\n\n", dir_name);

    struct stat sb;

    while ((entry = readdir(dir_fd)) != NULL) {

        char entry_type = dtype_char(entry->d_type);

        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);

        char* buf = (char*) calloc(BUF_SIZE, sizeof(char));
        get_access(sb.st_mode, buf);
        printf("[%s] ", buf);
        free(buf);

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        printf("%c %s\n", entry_type, entry->d_name);
    }

    closedir(dir_fd);
    return 0;
}