//! Compile with gcc -Wall -Wextra -o 1_stat 1_stat.c

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

//===============================================================

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSTAT_ONLY

char* get_UTC_time(char* str, const time_t* s_time) {

    //Указатель на структуру tm для хранения времени
    struct tm *tm_time;

    //считываем системное время и преобразуем системное время в UTC!!!!
    tm_time = gmtime(s_time);

    //Преобразуем время в текстовую строку
    if (strftime (str, 128, "%x %A %X (UTC)", tm_time) > 0) {
        return str;
    }
    else {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    // инициализируем структуру
    struct stat sb = {};

    #ifndef LSTAT_ONLY

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // добавил флаги на выбор системных вызовов:
    // "-s" stat
    // "-l" istat
    // "-f" fstat

    
    if (!strcmp(argv[1], "-s")) {
        puts("using system call stat");
        if (stat(argv[2], &sb) == -1) {
            perror("stat");
            return EXIT_FAILURE;
        }
    }
    else if (!strcmp(argv[1], "-l")) {
        puts("using system call lstat");
        if (lstat(argv[2], &sb) == -1) {
            perror("lstat");
            return EXIT_FAILURE;
        }
    }
    else if (!strcmp(argv[1], "-f")) {
        puts("using system call fstat");
        if (fstat(atoi(argv[2]), &sb) == -1) {
            perror("fstat");
            return EXIT_FAILURE;
        }   
    }
    #else
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }
    #endif

    // ID устройства состоит из 2х частей, которые содержатся в поле st_dev
    // функции major() и minor() возвращают эти части

    printf("ID of containing device:  [%lx,%lx]\n",
                    (long) major(sb.st_dev), (long) minor(sb.st_dev));

    printf("File type:                ");

    // маска S_IFMT путем побитового AND позволяет считывать только необходимые биты поля st_mode

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  puts("block device");            break;
    case S_IFCHR:  puts("character device");        break;
    case S_IFDIR:  puts("directory");               break;
    case S_IFIFO:  puts("FIFO/pipe");               break;
    case S_IFLNK:  puts("symlink");                 break;
    case S_IFREG:  puts("regular file");            break;
    case S_IFSOCK: puts("socket");                  break;
    default:       puts("unknown?");                break;
    }

    printf("I-node number:            %ld\n", (long) sb.st_ino);

    printf("Mode:                     %lo (octal)\n",
            (unsigned long) sb.st_mode);

    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Ownership:                UID=%ld   GID=%ld\n",
            (long) sb.st_uid, (long) sb.st_gid);

    printf("Preferred I/O block size: %ld bytes\n",
            (long) sb.st_blksize);
    printf("File size:                %lld bytes\n",
            (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n",
            (long long) sb.st_blocks);

    /*
    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
    */

    puts("===============================================");

    //Строка для сохранения преобразованного времени
    char str_t[128] = "";

    //Выводим строку в консоль
    printf ("Last status change:       %s\n", get_UTC_time(str_t, &sb.st_ctime));
    printf ("Last file access:         %s\n", get_UTC_time(str_t, &sb.st_atime));
    printf ("Last file modification:   %s\n", get_UTC_time(str_t, &sb.st_mtime));

    return EXIT_SUCCESS;
}

//* прошарить код
//===============================================================
//* заменил exit() на return
//* добавил флаги на выбор системных вызовов

/*

stat возвращает информацию о файле file_name и заполняет буфер buf. 

lstat идентична stat, но в случае символьных сылок она возвращает информацию о самой ссылке, а не о файле, на который она указывает.

fstat идентична stat, только возвращается информация об открытом файле, на который указывает filedes (возвращаемый open(2)), а не о file_name. Вызов open() используется, чтобы преобразовать путь к файлу в описатель файла (небольшое неотрицательно целое число, которое используется с вызовами read, write и т.п. при последующем вводе-выводе).

//===============================================================

statx возвращает более полную информацию о файле. Информация записывается в труктуру struct statx

*/

/*
st_dev;      устройство
st_ino;      inode
st_mode;     режим доступа
st_nlink;    количество жестких ссылок
st_uid;      ID пользователя-владельца
st_gid;      ID группы-владельца
st_rdev;     тип устройства
st_size;     общий размер в байтах
st_blksize;  размер блока ввода-вывода
st_blocks;   количество выделенных блоков
st_atime;    время последнего доступа
st_mtime;    время последней модификации
st_ctime;    время последнего изменения
*/