//* Compile with gcc -Wall -Wextra -o out main.c
#include <stdint.h> // uint8_t
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <assert.h>
#include <utime.h>
// --------------------------------------------------------------------------------------------
#include "../CT_tasks.h"

const unsigned int MAX_LEN = 1024 * 1024; // 1 Mbyte

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        puts("[err] Error message.");
        return RESULT_BAD_ARG;
    }

    // ===========================================================================================

    struct stat sb;

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    switch(sb.st_mode & S_IFMT) {
        case S_IFREG: {
            // проверяем право на чтение файла
            if (check_user_access(argv[1], 'r', sb.st_mode) != 1) {
                fprintf(stderr, "Usage: %s filename\n", argv[2]);
                perror("This file can't be read");
                return RESULT_BAD_READ;
            }

            // проверяем право на запись в файл
            if (check_user_access(argv[2], 'w', sb.st_mode) != 1) {
                fprintf(stderr, "Usage: %s filename\n", argv[2]);
                perror("This file can't be written in");
                return RESULT_BAD_WRITE;
            }

            // флаг __O_NOATIME используется, чтобы при открытии файла время доступа к нему не менялось
            int cp_file   = open(argv[1], O_RDONLY | __O_NOATIME);
            if (cp_file < 0) {
                perror("Failed for open copy file for writing");
                rm_file(argv[2]);
                return RESULT_OPEN_FAILED;
            }

            int dstn_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dstn_file < 0) {
                perror("Failed for open destination file for writing");
                rm_file(argv[2]);
                return RESULT_OPEN_FAILED;
            }

            copy_file(cp_file, dstn_file, argv[2], &sb, MAX_LEN);

            if (close(cp_file) < 0) {
                perror("Failed close copy file.");
                return RESULT_BAD_CLOSE;
            }

            if (close(dstn_file) < 0) {
                perror("Failed close destination file.");
                return RESULT_BAD_CLOSE;
            }

            break;
        }
        case S_IFIFO: {
            if (CopyFifo(argv[2], sb.st_mode) == -1) {
                perror("Failed to create FIFO file");
                return RESULT_ERR;
            }
            break;
        }
        case S_IFLNK: {
            CopyLink(argv[1], argv[2]);
            break;
        }
        case S_IFBLK: 
        case S_IFCHR: {
            if (CopyNod(argv[2], sb.st_mode, sb.st_rdev) == -1) {
                perror("Failed to create new character device");
                return RESULT_ERR;
            }
            break;
        }
        default: {
            perror("[err] Sorry, I can't copy this type of file.");
            return RESULT_BAD_FILE_TYPE;
        }
    }

    return RESULT_OK;
}
