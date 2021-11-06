//* Compile with gcc -Wall -Wextra -o out main.c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// --------------------------------------------------------------------------------------------
#include "../flib.h"
// размер буфера для форматного вывода времени в консоль
unsigned int BUF_SIZE = 64;

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
    printf ("Last status change:       %s\n", get_UTC_time(buf, &sb.st_ctime, BUF_SIZE));
    printf ("Last file access:         %s\n", get_UTC_time(buf, &sb.st_atime, BUF_SIZE));
    printf ("Last file modification:   %s\n", get_UTC_time(buf, &sb.st_mtime, BUF_SIZE));
        
    return EXIT_SUCCESS;
}