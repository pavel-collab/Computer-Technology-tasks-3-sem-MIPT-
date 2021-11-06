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
// --------------------------------------------------------------------------------------------
#include "../flib.h"

const unsigned int MAX_LEN = 1024 * 1024;

// в параметры командной строки передаются:
// файл-исток      argv[1]
// файл-назначение argv[2]

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

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        puts("[err] Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    // ===========================================================================================

    int cp_file   = open(argv[1], O_RDONLY);
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

    // ===========================================================================================
    
    // проверяем право на чтение файла
    if (check_user_access(argv[1], 'r', sb.st_mode) != 1) {
        fprintf(stderr, "Usage: %s filename\n", argv[2]);
        perror("This file can't be read");
        return RESULT_BAD_READ;
    }

    // // проверяем право на запись в файл
    // if (check_user_access(argv[2], 'w') != 1) {
    //     fprintf(stderr, "Usage: %s filename\n", argv[2]);
    //     perror("This file can't be written in");
    //     return RESULT_BAD_WRITE;
    // }

    // ===========================================================================================
    
    copy_reg_file(cp_file, dstn_file, argv[2], &sb, MAX_LEN);

    // ===========================================================================================

    if (close(cp_file) < 0) {
        perror("Failed close copy file.");
        return RESULT_BAD_CLOSE;
    }

    if (close(dstn_file) < 0) {
        perror("Failed close destination file.");
        return RESULT_BAD_CLOSE;
    }

    return RESULT_OK;
}
