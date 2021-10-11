//* Compile with gcc -Wall -Wextra -o out main.c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../enum.h"

// размер буфера для форматного вывода времени в консоль
unsigned int BUF_SIZE = 64;

// считывает системное время и преобразует его в строку; время выводится в UTC
char* get_UTC_time(char* str, const time_t* s_time) {
    //Указатель на структуру tm для хранения времени
    struct tm *tm_time;

    //считываем системное время и преобразуем системное время в UTC!!!!
    tm_time = gmtime(s_time);
    return (strftime (str, BUF_SIZE, "%x %A %X (UTC)", tm_time) > 0) ? str : NULL;
}

const char* file_type(mode_t mode) {
    // маска S_IFMT путем побитового AND позволяет считывать только необходимые биты поля st_mode
    // man 2 stat
    switch (mode & S_IFMT) {
        case S_IFBLK:  return "block device";            break;
        case S_IFCHR:  return "character device";        break;
        case S_IFDIR:  return "directory";               break;
        case S_IFIFO:  return "FIFO/pipe";               break;
        case S_IFLNK:  return "symlink";                 break;
        case S_IFREG:  return "regular file";            break;
        case S_IFSOCK: return "socket";                  break;
        default:       return "unknown?";                break;
    }
}

// записываем в buf права доступа к файлу
void get_access(mode_t st_mode, char* buf) {
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

int main(int argc, char *argv[])
{
    struct stat sb;

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

    printf("File type:                %s\n", file_type(sb.st_mode));    

    // права доступа
    // см шпаргалку в Readme или man 2 stat, man 2 lstat
    printf("access:                   %o/", sb.st_mode & 1023);
            

    char buf[BUF_SIZE];
    get_access(sb.st_mode, buf);
    printf("%s\n", buf);

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


    //Выводим строку в консоль
    printf ("Last status change:       %s\n", get_UTC_time(buf, &sb.st_ctime));
    printf ("Last file access:         %s\n", get_UTC_time(buf, &sb.st_atime));
    printf ("Last file modification:   %s\n", get_UTC_time(buf, &sb.st_mtime));
        
    return EXIT_SUCCESS;
}