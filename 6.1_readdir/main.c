//* Compile with gcc -Wall -Wextra -o out main.c
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "../enum.h"

char file_mode(unsigned mode) {

    switch (mode & S_IFMT) {
        case S_IFBLK:  return 'b';   break;
        case S_IFCHR:  return 'c';   break;
        case S_IFDIR:  return 'd';   break;
        case S_IFIFO:  return 'f';   break;
        case S_IFLNK:  return 'l';   break;
        case S_IFREG:  return 'r';   break;
        case S_IFSOCK: return 's';   break;
        default:       return '?';   break;
    }

    return '?';

}

char dtype_char(unsigned char dtype) {

    switch (dtype) {
        case DT_BLK :     return 'b'; break; 
        case DT_CHR:      return 'c'; break;
        case DT_DIR:      return 'd'; break;
        case DT_FIFO:     return 'f'; break;
        case DT_LNK:      return 'l'; break;
        case DT_REG:      return 'r'; break;
        case DT_SOCK:     return 's'; break;
        case DT_UNKNOWN:  return '?'; break;
    }

    return '?';
}

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

        printf("[%c%c%c%c%c%c%c%c%c] ",
                sb.st_mode & S_IRUSR ? 'r' : '-',
                sb.st_mode & S_IWUSR ? 'w' : '-',
                sb.st_mode & S_IXUSR ? 'x' : '-',
                sb.st_mode & S_IRGRP ? 'r' : '-',
                sb.st_mode & S_IWGRP ? 'w' : '-',
                sb.st_mode & S_IXGRP ? 'x' : '-',
                sb.st_mode & S_IROTH ? 'r' : '-',
                sb.st_mode & S_IWOTH ? 'w' : '-',
                sb.st_mode & S_IXOTH ? 'x' : '-');

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        printf("%c %s\n", entry_type, entry->d_name);
    }

    closedir(dir_fd);
    return RESULT_OK;
}