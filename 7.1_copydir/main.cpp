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
#include "../CT_tasks.h"
#include <iostream>
#include <string.h>

const char* CopyFileName(const char* filename) {
    std::string filename_s(filename, strlen(filename));
    std::string copy_file_name = (std::string) "Copy_" + filename_s;
    return copy_file_name.c_str();
}

const unsigned int MAX_LEN = 1024 * 1024;

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

    DIR* dir_fd = fdopendir(sys_dir_fd);

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
    printf("Good open the CopyDir\n");

    // --------------------------------------------------------------------------------------------

    struct dirent* entry;
    // в цикле начинаем читать записи из копируемого каталога
    //! (копируем только обычные файлы)
    while ((entry = readdir(dir_fd)) != NULL) {
        printf("Enter into the loop\n");
        struct stat sb;
        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);
        
        // спомощью stat проверяем тип файла
        if ((sb.st_mode & S_IFMT) != S_IFREG) {
            perror("[err] Sorry, I can't copy this type of file this moment(");
        }
        else {
            // открываем копируемый файл
            int cp_file = openat(sys_dir_fd, entry->d_name, O_RDONLY);
            if (cp_file < 0) {
                perror("Failed for open copy file for writing");
                rm_file(argv[2]);
                return RESULT_OPEN_FAILED;
            }
            printf("good regular file\n");
            // открываем (создаем) файл-копию в каталоге /base_dir/copy_dir
            int dstn_file = openat(sys_copy_dir, entry->d_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dstn_file < 0) {
                perror("Failed for open destination file for writing");
                rm_file(argv[2]);
                return RESULT_OPEN_FAILED;
            }
            printf("create file - copy\n");
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
        }
    }

    close(sys_copy_dir);
    closedir(dir_fd);
    close(base_dir);
    return RESULT_OK;
}