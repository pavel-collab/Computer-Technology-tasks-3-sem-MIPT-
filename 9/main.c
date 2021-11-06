#include <stdio.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void FileSystemInfo(const char* fs_name, struct statvfs* sbfs) {
    printf("Usage: %s\n\n", fs_name);

    printf("Filesystem block size:                          %lu\n", sbfs->f_bsize);
    printf("Fragment size:                                  %lu\n", sbfs->f_frsize);
    printf("Size of filesystem in fragment size units:      %lu\n", sbfs->f_blocks);
    printf("Number of free blocks:                          %lu\n", sbfs->f_bfree);
    printf("Number of free blocks for unprivileged users:   %lu\n", sbfs->f_bavail);
    printf("Number of inodes:                               %lu\n", sbfs->f_files);
    printf("Number of free inodes:                          %lu\n", sbfs->f_ffree);
    printf("Number of free inodes for unprivileged users:   %lu\n", sbfs->f_favail);
    printf("Filesystem ID:                                  %lu\n", sbfs->f_fsid);
    printf("Mount flags:                                    %lu\n", sbfs->f_flag);
    printf("Maximum filename length:                        %lu\n", sbfs->f_namemax);

}

// the first argument is a file system
// the second argument is a file

// if user input only one argument, the default file system will be a current folder

// the program output information about file system was input and answer does the file fit to the file system

int main(int argc, char* argv[]) {

    char* fs_name = ".";
    struct statvfs sbfs;

    char* file_name;
    struct stat sb;

    if (argc < 2) {
        perror("expected more arguments");
        return -1;
    }

    if (argc == 2) {
        file_name = argv[1];
    }
    else if (argc == 3) {
        fs_name = argv[1];
        file_name = argv[2];
    }

    if (lstat(fs_name, &sb) < 0) {
        perror("lstat");
        return 0;
    }

    // Check the file system name. If the type of this object is not a dir there wiil be an error
    if ((sb.st_mode & S_IFMT) != S_IFDIR) {
        fprintf(stderr, "[err] %s isn't a file system\n", fs_name);
        return -1;
    }

    if (statvfs(fs_name, &sbfs) < 0) {
        perror("statvfs");
        return -1;
    }

    if (lstat(file_name, &sb) < 0) {
        perror("lstat");
        return 0;
    }

    FileSystemInfo(fs_name, &sbfs);

    /* fsfilcnt_t    f_favail -- Number of free inodes for unprivileged users */
    /* unsigned long f_bsize  -- Filesystem block size                        */
    /* fsblkcnt_t    f_bavail -- Number of free blocks for unprivileged users */
    if ((sbfs.f_favail < 1) || ((long int) (sbfs.f_bavail * sbfs.f_bsize) < sb.st_size)) {
        printf("\nThe file %s doesn't fit to %s\n", file_name, fs_name);
    }
    else {
        printf("\nThe file %s fits to %s\n", file_name, fs_name);
    }

    return 0;
}