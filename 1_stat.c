//* Compile with gcc -Wall -Wextra -o 1_stat 1_stat.c

// заголовочные файлы для вызова stat, lstat, fsltat
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        return NULL;
    }
}

int main(int argc, char *argv[])
{
    // инициализируем структуру
    struct stat sb = {};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        return EXIT_FAILURE;
    }

    printf("File:                     %s\n", argv[1]);
    
    // ID устройства состоит из 2х частей, которые содержатся в поле st_dev
    // функции major() и minor() возвращают эти части

    printf("ID of containing device:  [%lx,%lx]\n",
                    (long) major(sb.st_dev), (long) minor(sb.st_dev));

    printf("File type:                ");

    // маска S_IFMT путем побитового AND позволяет считывать только необходимые биты поля st_mode
    // man 2 stat

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
    // см шпаргалку в Readme или man 2 stat, man 2 lstat

    // the first variant output access in octal sign
    // printf("access        :            %o/", sb.st_mode & 1023);

    // the second variant output access in octal sign
    printf("Режим доступа:            %lo (octal)/",
            (unsigned long) sb.st_mode);
            
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

//TODO: refactoring: man 3 printf
//TODO: refactoring: function get_UTC_time
//TODO: refactoring: опечатки в Readme????

//* разобарться с правами доступа -> man inode(7)
//TODO: пересмотреть параметры strftime
//TODO: man statx(2)
//TODO: заполнить Readme по таску