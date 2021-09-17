//! Compile with gcc -Wall -Wextra -o 1_stat 1_stat.c

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSTAT_ONLY

// считывает системное время и преобразует его в строку (время в UTC)
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
        exit(1);
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

    printf("File:                     %s\n", argv[1]);

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

    // права доступа

    int acccess_rights = 0;

    // см шпаргалку в Readme

    acccess_rights += sb.st_mode & S_IRUSR ? 400 : 0;
    acccess_rights += sb.st_mode & S_IRGRP ? 40  : 0;
    acccess_rights += sb.st_mode & S_IROTH ? 4   : 0;

    acccess_rights += sb.st_mode & S_IWUSR ? 200 : 0;
    acccess_rights += sb.st_mode & S_IWGRP ? 20  : 0;
    acccess_rights += sb.st_mode & S_IWOTH ? 2   : 0;

    acccess_rights += sb.st_mode & S_IXUSR ? 100 : 0;
    acccess_rights += sb.st_mode & S_IXGRP ? 10  : 0;
    acccess_rights += sb.st_mode & S_IXOTH ? 1   : 0;

    printf("access rights:            0%d/", acccess_rights);

    printf("%c%c%c%c%c%c%c%c%c\n",
        sb.st_mode & S_IRUSR ? 'r' : '-',
        sb.st_mode & S_IWUSR ? 'w' : '-',
        sb.st_mode & S_IXUSR ? 'x' : '-',
        sb.st_mode & S_IRGRP ? 'r' : '-',
        sb.st_mode & S_IWGRP ? 'w' : '-',
        sb.st_mode & S_IXGRP ? 'x' : '-',
        sb.st_mode & S_IROTH ? 'r' : '-',
        sb.st_mode & S_IWOTH ? 'w' : '-',
        sb.st_mode & S_IXOTH ? 'x' : '-');

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

    //Строка для сохранения преобразованного времени
    char str_t[128] = "";

    //Выводим строку в консоль
    printf ("Last status change:       %s\n", get_UTC_time(str_t, &sb.st_ctime));
    printf ("Last file access:         %s\n", get_UTC_time(str_t, &sb.st_atime));
    printf ("Last file modification:   %s\n", get_UTC_time(str_t, &sb.st_mtime));

    return EXIT_SUCCESS;
}

//* разобарться с правами доступа -> man inode(7)
//TODO: пересмотреть параметры strftime
//TODO: man statx(2)
//TODO: заполнить Readme по таску