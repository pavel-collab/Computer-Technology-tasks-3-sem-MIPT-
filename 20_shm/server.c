//* gcc -o server server.c -lrt
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage %s shm-name text", argv[0]);
        return -1;
    }

    int fd = shm_open(argv[1], O_RDWR | O_CREAT, 0666); /* Open existing object */
    if (fd == -1) {
        perror("shm_open");
        return -1;
    }

    int len = strlen(argv[2]);
    if (ftruncate(fd, len) == -1) { /* Resize object to hold string */
        perror("ftruncate()");
        return -1;
    }
    
    printf("Resized to %ld bytes\n", (long) len);
    
    void* addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap()");
        return -1;
    }

    if (close(fd) == -1) { /* 'fd' is no longer needed */
        perror("close()");
        return -1;
    }

    printf("copying %ld bytes\n", (long) len);
    memcpy(addr, argv[2], len); /* Copy string to shared memory */

    // удаляем область разделяемой памяти
    // shm_unlink(argv[1]);
    return 0;
}

// посмотреть на созданную область разделяемой памяти:
// ls -l /dev/shm 