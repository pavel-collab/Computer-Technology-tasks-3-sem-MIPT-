#define _XOPEN_SOURCE_EXTENDED
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <assert.h>
#include "enum.h"

#include <dirent.h>
#include <string.h>

#include <errno.h>

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

void GetAccess(unsigned st_mode, char* buf) {
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

char* GetCurrentDirPath(char* buf, unsigned buf_size) {

    // unsigned buf_size = PATH_MAX;
    // char* buf = (char*) malloc(buf_size);
    // assert(buf != NULL);

    // strcpy(buf, dir_name); // скопировать строку dir_name в buf

    char* getcwd_result = getcwd(buf, buf_size);

    // если получилось так, что абсолютный путь к текущему каталогу
    // больше buf_size, расширяем буфер до тех пор,
    // пока абсолютный путь к текущему каталогу не влезет в него
    while ((getcwd_result == NULL) && (errno == ERANGE)) {
        buf_size = buf_size * 2;
        buf = (char*) realloc(buf, buf_size * sizeof(char));

        getcwd_result = getcwd(buf, buf_size);
    }

    // если после этого функция все еще возвращает NULL, 
    // значит имеют место ошибки другого характера
    if (getcwd_result == NULL) {
        perror("Can't get dir path");
        free(buf);
        return NULL;
    }
    return getcwd_result;
}

int SearchDir(char* dir_name, int level) {

    int sys_dir_fd = open(dir_name, O_RDONLY);

    if (sys_dir_fd < 0) {
        perror("Failed for open file");
        return RESULT_OPEN_FAILED;
    }

    DIR* dir_fd = fdopendir(sys_dir_fd);
    //puts("DIR* dir_fd = opendir(dir_name);");
    if (dir_fd == NULL) {
        perror("Bad dir file descriptor");
        return RESULT_ERR;
    }

    struct dirent* entry;
    // printf("dir: %s\n\n", dir_name);

    while ((entry = readdir(dir_fd)) != NULL) {

        char entry_type = dtype_char(entry->d_type);

        struct stat sb;
        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);

        char* buf = (char*) calloc(9, sizeof(char));
        assert(buf != NULL);

        GetAccess(sb.st_mode, buf);
        printf("%*s ", (int) (level + strlen(buf)), buf);
        free(buf);

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        printf("%c %s\n", entry_type, entry->d_name);

        if ((entry_type == 'd') && (strcmp(entry->d_name, "..")) && (strcmp(entry->d_name, ".")) && (strcmp(entry->d_name, ".git"))) {
            // printf("CATCH A NEW DIR: %s\n\n", entry->d_name);

            if (chdir(entry->d_name) < 0) {
                perror("It's not possible to change directory now");
                return RESULT_ERR;
            }

            char* buf = (char*) calloc(PATH_MAX, sizeof(char));
            char* abs_dir_path = GetCurrentDirPath(buf, PATH_MAX);
            assert(abs_dir_path != NULL);

            // printf("current dir: [%s]\n", abs_dir_path);
            
            level+=4;
            SearchDir(abs_dir_path, level);
            level-=4;
            free(buf);

            if (chdir("..") < 0) {
                perror("It's not possible to change directory now");
                return RESULT_ERR;
            }
        }
    }

    closedir(dir_fd);
    return RESULT_OK;
}

int main(int argc, char* argv[]) {

    char* dir_name = ".";
    if (argc == 2) {
        dir_name = argv[1];

        if (chdir(dir_name) < 0) {
            perror("It's not possible to change directory now");
            return RESULT_ERR;
        }
    }

    SearchDir(dir_name, 0);   
    return 0;
}