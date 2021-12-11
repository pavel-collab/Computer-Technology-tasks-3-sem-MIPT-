//* gcc -o client client.c -lrt
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
    int fd;
    char *addr;
    struct stat sb;

    if (argc != 2){
        fprintf(stderr, "Usage %s shm-name", argv[0]);
        return -1;
    }
    
    fd = shm_open(argv[1], O_RDONLY, 0); /* Open existing object */
    if (fd == -1) {
        perror("shm_open");
        return -1;
    }

    /* Use shared memory object size as length argument for mmap()
    and as number of bytes to write() */
    if (fstat(fd, &sb) == -1) {
        perror("fstat()");
        return -1;
    }

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap()");
        return -1;
    }

    if (close(fd) == -1) { /* 'fd' is no longer needed */
        perror("close()");
        return -1;
    }

    write(fileno(stdout), addr, sb.st_size);
    printf("\n");
    
    return 0;
}