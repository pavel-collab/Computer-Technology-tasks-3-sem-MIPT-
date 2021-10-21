#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <errno.h>
// --------------------------------------------------------------------------------------------
#include <iostream>
#include <string.h>

#include "../CT_tasks.h"

const unsigned int MAX_LEN = 1024 * 1024;

const char* CopyFileName(const char* filename) {
    std::string filename_s(filename, strlen(filename));
    std::string copy_file_name = (std::string) "Copy_" + filename_s;
    return copy_file_name.c_str();
}

// sys_dir_fd     -- файловый дескрриптор копируемого каталога
// sys_copy_dir   -- файловый дескриптор каталога-копии
int CopyDir(int sys_dir_fd, int sys_copy_dir) {
    DIR* dir_fd = fdopendir(sys_dir_fd);
    struct dirent* entry;

    // в цикле начинаем читать записи из копируемого каталога
    //! (копируем только обычные файлы)
    while ((entry = readdir(dir_fd)) != NULL) {
        struct stat sb;
        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);
        
        switch (sb.st_mode & S_IFMT) {
            case S_IFREG: {
                // открываем копируемый файл
                int cp_file = openat(sys_dir_fd, entry->d_name, O_RDONLY);
                if (cp_file < 0) {
                    perror("Failed for open copy file for writing");
                    return RESULT_OPEN_FAILED;
                }
                // открываем (создаем) файл-копию в каталоге /base_dir/copy_dir
                int dstn_file = openat(sys_copy_dir, entry->d_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (dstn_file < 0) {
                    perror("Failed for open destination file for writing");
                    return RESULT_OPEN_FAILED;
                }
                // ===========================================================================================
                
                // проверяем право на чтение файла
                if (check_user_access(entry->d_name, 'r', sb.st_mode) != 1) {
                    perror("This file can't be read");
                    return RESULT_BAD_READ;
                }

                // ===========================================================================================
                // производим копирование
                copy_reg_file(cp_file, dstn_file, entry->d_name, &sb, MAX_LEN);

                // ===========================================================================================        

                // закрываем файлы
                close(cp_file);
                close(dstn_file);
                break;
            }
            case S_IFDIR: {
                if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
                    break;
                }
                
                // открываем копируемый каталог
                int sys_dir_fd_lowl = openat(sys_dir_fd, entry->d_name, O_RDONLY);
                if (sys_dir_fd_lowl < 0) {
                    perror("Can't open current directory");
                    return RESULT_OPEN_FAILED;
                }

                struct stat dir_sb;
                assert((fstatat(sys_dir_fd, entry->d_name, &dir_sb, AT_SYMLINK_NOFOLLOW)) == 0);
                // --------------------------------------------------------------------------------------------
                
                // создаем в скопированной дериктории каталог-копию с правами исходника
                if (mkdirat(sys_copy_dir, CopyFileName(entry->d_name), dir_sb.st_mode) < 0) {
                    perror("Failed to create directory");
                    return RESULT_ERR;
                }

                // --------------------------------------------------------------------------------------------
                
                // открываем каталог-копию
                int sys_copy_dir_lowl = openat(sys_copy_dir, CopyFileName(entry->d_name), O_RDONLY);
                if (sys_copy_dir_lowl == -1) {
                    perror("Can't open CopyDir directory");
                    printf("errno = %d\n", errno);
                    return RESULT_OPEN_FAILED;
                }

                // --------------------------------------------------------------------------------------------

                CopyDir(sys_dir_fd_lowl, sys_copy_dir_lowl);
                break;
            }
            default: {
                fprintf(stderr, "[err] File ---%s--- has type [%c], it's not possible to copy it now\n", entry->d_name, file_mode(sb.st_mode));
                break;
            }
        }
    }
    closedir(dir_fd);
    return RESULT_OK;
}

// argv[1] -- путь к родительскому каталогу, где надодится копируемая дериктория
// argv[2] -- имя копируемой дериктории

int main(int argc, char* argv[]) {

    // dir_fd   -- копируемая дериктория
    // base_dir -- родительская дериктория

    if (argc < 3) {
        perror("expected more arguments");
        return RESULT_ERR;
    }

    // --------------------------------------------------------------------------------------------

    // открываем родительскую дерикторию, где находится копируемый каталог
    int base_dir = open(argv[1], O_RDONLY, O_DIRECTORY);
    if (!base_dir) {
        perror("Can't open current directory");
        return RESULT_OPEN_FAILED;
    }
    
    // --------------------------------------------------------------------------------------------
    
    // открываем копируемый каталог
    int sys_dir_fd = openat(base_dir, argv[2], O_RDONLY);
    if (sys_dir_fd < 0) {
        perror("Can't open current directory");
        return RESULT_OPEN_FAILED;
    }

    struct stat dir_sb;
    assert((fstatat(base_dir, argv[2], &dir_sb, AT_SYMLINK_NOFOLLOW)) == 0);
    // --------------------------------------------------------------------------------------------
    
    // создаем в родительской дериктоии каталог-копию с правами исходника
    if (mkdirat(base_dir, CopyFileName(argv[2]), dir_sb.st_mode) < 0) {
        perror("Failed to create directory");
        return RESULT_ERR;
    }

    // --------------------------------------------------------------------------------------------
    
    // открываем каталог-копию
    int sys_copy_dir = openat(base_dir, CopyFileName(argv[2]), O_RDONLY);
    if (sys_copy_dir == -1) {
        perror("Can't open CopyDir directory");
        printf("errno = %d\n", errno);
        return RESULT_OPEN_FAILED;
    }

    // --------------------------------------------------------------------------------------------
    
    // производим копирование
    CopyDir(sys_dir_fd, sys_copy_dir);

    //! ATTENTION мы не закрываем файловый дескриптор sys_dir_fd, потому что в функции CopyDir 
    //! мы закрыли дискриптор dir_fd
    close(sys_copy_dir);
    close(base_dir);

    assert(errno == 0);
    return RESULT_OK;
}