//* Compile with: gcc -Wall -Wextra -o copy 3_task.c

#include <stdint.h> // uint8_t

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include <assert.h>

#include <utime.h>

const unsigned int MAX_LEN = 256;
const size_t buf_size = 256;

#include "enum.h"

ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = buf;

    while (bytes_written < count) {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);

        if (res < 0) {
            return res;
        }

        bytes_written += (size_t) res;
    }
    return (ssize_t) bytes_written;
}

// the function of the removing fire from the current dir
int rm_file(char* filename) {

    // конкатенация строк
    const char *rm  = "rm ";

    char command[512];
    snprintf(command, sizeof command, "%s%s", rm, filename);
    // ----------------------------------------------------------------------------------------

    system(command);
    printf("[+] Successful removing: %s", filename);

    return 0;
}


int copy_file(char* copy_file, char* destination_file) {

    int cp_file   = open(copy_file, O_RDONLY);
    int dstn_file = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (cp_file < 0) {
        perror("Failed for open copy file for writing");
        rm_file(destination_file);
        return RESULT_OPEN_FAILED;
    }

    if (dstn_file < 0) {
        perror("Failed for open destination file for writing");
        rm_file(destination_file);
        return RESULT_OPEN_FAILED;
    }

    struct stat sb = {};

    if (lstat(copy_file, &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // узнаем размер файла в байтах
    long long copy_file_size = (long long) sb.st_size;

    char* buf = (char*) calloc(MAX_LEN, sizeof(char));
    assert(buf != NULL);

    while(copy_file_size > 0) {

        ssize_t read_symb_amount = read(cp_file, buf, MAX_LEN);

        if (read_symb_amount < 0) {
            perror("Failed read from the file");
            rm_file(destination_file);
            close(cp_file);
            return RESULT_BAD_READ;
        }

        ssize_t write_symb_amount = writeall(dstn_file, buf, read_symb_amount);

        if (write_symb_amount < 0) {
            perror("Failed write to file");
            rm_file(destination_file);
            close(dstn_file);
            return RESULT_BAD_WRITE;
        }

        if (write_symb_amount != read_symb_amount) {
            perror("The number of symbols written does't match the number of symbols read.");
            rm_file(destination_file);
            close(dstn_file);
            return RESULT_BAD_WRITE;
        }

        copy_file_size -= read_symb_amount;

    }

    // инициализируем структуру
    struct utimbuf file_time_buf = {};

    // кладем в стуктуру время доступа и модификации исходного файла
    file_time_buf.actime = sb.st_atime;
    file_time_buf.modtime = sb.st_mtime;

    // присваиваем новому файлу параметры времени исходника
    if (utime(destination_file, &file_time_buf) != 0) {
        perror("Error, impossible to assign the values of access time and modefite time.");
        rm_file(destination_file);
        close(dstn_file);
        return RESULT_BAD_COPY_TIME;
    }

    //-----------------------------------------------------------------------------------------
    
    //? Ура, теперь я могу копировать исполняемые файлы
    //? Но правильно ли я это сделал?
    fchmod(dstn_file, sb.st_mode);

    if (close(cp_file) < 0) {
        perror("Failed close copy file.");
        return RESULT_BAD_CLOSE;
    }

    if (close(dstn_file) < 0) {
        perror("Failed close destination file.");
        return RESULT_BAD_CLOSE;
    }

    free(buf);

    return RESULT_OK;
}


// в параметры командной строки передаются:
// файл-исток      argv[1]
// файл-назначение argv[2]

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        puts("[err] Error message.");
        return RESULT_BAD_ARG;
    }

    // инициализируем структуру
    struct stat sb = {};

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    // спомощью stat проверяем тип файла
    if ((sb.st_mode & S_IFMT) != S_IFREG) {
        puts("[err] Sorry, I can't copy this type of file this moment(");
        return RESULT_BAD_FILE_TYPE;
    }

    copy_file(argv[1], argv[2]);

    return RESULT_OK;
}

//! Когда мы копируем файл, мы "трогаем его", поэтому время доступа меняется на текущую дату,
//! а в копию мы записываем "старое" значение времени доступа
//! как итог, совпадает только время модификации
//* read man 2 open -> как открыть файл, чтобы его не "потрограть" искать по a_time
//* man 2 unlink -> удалить ссылку на объект, когда количесво ссылок упадет до 0 
//* система сама подберет мусор --- что это значит?

//* посмотреть, какие системные вызовы дергает утилита remove
//! читать полностью маны на chown and chmod!!!

//* если копировать собственные файлы, то в chown нет никакого смысла.

//? mknode
//? mkfifo
// считываем содержимое ссылки с помощью readlink
// и записываем ее содержимое в новую ссылку с помощью symlink
