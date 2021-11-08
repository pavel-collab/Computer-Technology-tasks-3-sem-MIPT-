#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <time.h>

// размер буфера для форматного вывода времени в консоль
unsigned int BUF_SIZE = 64;

//TODO: understand, why the dir path is printed more that one times --> fix
// --------------------------------------------------------------------------------------------
//TODO: check the problem with rename of file --> solve problem

/*  Read all available inotify events from the file descriptor "fd"

    wd   -- is the table of watch descriptors for the directories in argv.
    argc -- is the length of wd and argv.
    argv --  is the list of watched directories.
*/

char* get_UTC_time(char* str, const time_t* s_time, unsigned buf_size) {
    //Указатель на структуру tm для хранения времени
    struct tm *tm_time;

    //считываем системное время и преобразуем системное время в UTC!!!!
    tm_time = gmtime(s_time);
    return (strftime (str, buf_size, "%x %A %X (UTC)", tm_time) > 0) ? str : NULL;
}

void handle_events(int fd, int *wd, int argc, char* argv[]) {

    char time_buf[BUF_SIZE];

    /*  the structure inotify must be read all, 
        so it's necessary allocate enough buf size */
    char buf[4096];
    const struct inotify_event *event;
    int i;
    ssize_t len;
    char *ptr;

    /* Loop while events can be read from inotify file descriptor. */
    for (;;) {

        /* read some events */
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            return;
        }
        /*  If the nonblocking read() found no events to read, then
            it returns -1 with errno set to EAGAIN. In that case,
            we exit the loop. */


        if (len <= 0)
            break;

        //* man inotify(7) 
        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;

            time_t cur_time = time(NULL);

            printf("%s | ", get_UTC_time(time_buf, &cur_time, BUF_SIZE));

            /* print event type */
            if (event->mask & IN_OPEN)
                printf("IN_OPEN:          ");
            if (event->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE: ");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE:   ");
            if (event->mask & IN_ACCESS)
                printf("IN_ACCESS:        ");
            if (event->mask & IN_CREATE)
                printf("IN_CREATE:        ");
            if (event->mask & IN_DELETE)
                printf("IN_DELETE:        ");
            if (event->mask & IN_MODIFY)
                printf("IN_MODIFY:        ");
            if (event->mask & IN_ATTRIB)
                printf("IN_ATTRIB:        ");
            

            /* print the name of the watched directory */
            for (i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s/", argv[i]);
                    break;
                }
            }

            /* print the file name */
            if (event->len)
                printf("%s", event->name);

            /* print the name of filesystem object */
            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");
        }
    }
}

int main(int argc, char* argv[]) {

    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;

    struct pollfd fds[2];

    if (argc < 2) {
        printf("Usage: %s PATH [PATH ...]\n", argv[0]);
        return -1;
    }

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API */
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        return -1;
    }

    /* Allocate memory for watch descriptors */
    wd = calloc(argc, sizeof(int));
    if (wd == NULL) {
        perror("calloc");
        return -1;
    }

    /* Mark directories for events
            - file was opened
            - file was closed 
            - there was an access to the file (read for example)
            - in watched directory was created a new file
            - in watched directory was deleted a file
            - file was modified
            - meta data of file was modified */
    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i],
                                    IN_OPEN | IN_CLOSE |
                                    IN_ACCESS |
                                    IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB);
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch %s\n", argv[i]);
            perror("inotify_add_watch");
            return -1;
        }
    }

    /* Prepare for polling */
    nfds = 2;

    //* man poll(2)
    fds[0].fd = STDIN_FILENO; // stdin
    fds[0].events = POLLIN; // watched event -- input

    fds[1].fd = fd; // inotify
    fds[1].events = POLLIN; // watched event -- input

    /* Wait for events and/or terminal input */
    printf("Waiting for events\n\n");
    while (1) {

        poll_num = poll(fds, nfds, -1); // 2 -- amount of file descriptions; -1 -- infinity waiting time

        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            return -1;
        }

        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {
                /* Console input is available. Empty stdin and quit */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {
                /* Inotify events are available */
                puts("-------------------------------------------------------------------------------------------");
                handle_events(fd, wd, argc, argv);
                puts("-------------------------------------------------------------------------------------------");
            }
        }
    }
    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */
    close(fd);
    free(wd);

    return -1;
}
