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

// записываем в buf 
void get_access(unsigned st_mode, char* buf) {
    buf[0] = st_mode & S_IRUSR ? 'r' : '-';
    buf[1] = st_mode & S_IWUSR ? 'w' : '-';
    buf[2] = st_mode & S_IXUSR ? 'x' : '-';
    buf[3] = st_mode & S_IRGRP ? 'r' : '-';
    buf[4] = st_mode & S_IWGRP ? 'w' : '-';
    buf[5] = st_mode & S_IXGRP ? 'x' : '-';
    buf[6] = st_mode & S_IROTH ? 'r' : '-';
    buf[7] = st_mode & S_IWOTH ? 'w' : '-';
    buf[8] = st_mode & S_IXOTH ? 'x' : '-';
}

// (оформление) функция печатает отступы в соответсвии с вложенностью файла
void PrintLevel(int level) {
    while (level-- > 0) {
        printf("-");
    }
}

// вывод дериктории в консоль
int PrintDir(char* buf, char entry_type, char* file_name, int level) {
    PrintLevel(level);
    fprintf(stdout, " | %s %c %s\n", buf, entry_type, file_name);
    return RESULT_OK;
}

// функция проверяет, можно ли безопасно читать данный каталог
int check_dir(char entry_type, char* dr_name, mode_t mode) {
    // проверяем, что имеем дело с каталогом
    if (entry_type != 'd') {
        return NOT_DIR;
    }
    // проверяем право на чтение
    if (!(mode & S_IRUSR)) {
        return RESULT_BAD_READ;
    }

    // проверяем, что это не текущий и не родительский каталог
    if ((strcmp(dr_name, "..")) && (strcmp(dr_name, "."))) {
        #ifdef DOT_GIT_VEIW
            return SUITABLE_DIR;
        #else
            if (strcmp(dr_name, ".git")) {
                return SUITABLE_DIR;
            }
        #endif
    }

    return -1;
}

int SearchDir(int sys_dir_fd, int level) {

    // получаем C-шный файловый дескриптор из системного
    DIR* dir_fd = fdopendir(sys_dir_fd);

    if (dir_fd == NULL) {
        perror("Bad dir file descriptor");
        return RESULT_ERR;
    }

    struct dirent* entry;
    struct stat sb;

    while ((entry = readdir(dir_fd)) != NULL) {

        char entry_type = dtype_char(entry->d_type);
        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        char* buf = (char*) calloc(9, sizeof(char));
        assert(buf != NULL);
        get_access(sb.st_mode, buf);
        PrintDir(buf, entry_type, entry->d_name, level);
        free(buf);

        if (check_dir(entry_type, entry->d_name, sb.st_mode) == SUITABLE_DIR) {
            int new_dir_fd = openat(sys_dir_fd, entry->d_name, O_RDONLY);

            level+=4;
            SearchDir(new_dir_fd, level);
            level-=4;
            close(new_dir_fd);
        }
    }

    closedir(dir_fd);
    return RESULT_OK;
}

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

    SearchDir(sys_dir_fd, 0); 
    close(sys_dir_fd);  
    return 0;
}