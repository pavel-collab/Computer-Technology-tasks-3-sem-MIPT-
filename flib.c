#include <stdint.h> // uint8_t
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <assert.h>
#include <utime.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
// --------------------------------------------------------------------------------------------
#include "flib.h"

// функци считывает системное время и преобразует его в строку; время выводится в UTC
// параметры функции :
//                    str      --
//                    s_time   --
//                    buf_size --
char* get_UTC_time(char* str, const time_t* s_time, unsigned buf_size) {
    //Указатель на структуру tm для хранения времени
    struct tm *tm_time;

    //считываем системное время и преобразуем системное время в UTC!!!!
    tm_time = gmtime(s_time);
    return (strftime (str, buf_size, "%x %A %X (UTC)", tm_time) > 0) ? str : NULL;
}

// функция определяет тип файла (regulaar, dir, symlink etc)
// параметры функии:
//                  mode --
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

// функция записывает в буффер buf права доступа к файлу
// параметры функции:
//                    st_mode --
//                    buf     --
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

// функция записывает в файл с файловым дискриптором fd содержимое буффера buf
// параметры функции:
//                   fd    --
//                   buf   --
//                   count --
ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t bytes_written = 0;
    const uint8_t *buf_addr = (const uint8_t*) buf;

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
int rm_file(const char* filename) {

    if (unlink(filename) == -1) {
        perror("It's not possible to remove this file");
        return RESULT_ERR;
    }
    printf("[+] Successful removing: %s", filename);
    return RESULT_OK;
}

// функция проверяет права пользователя
int check_user_access(const char* file_name, const char access, mode_t mode) {

    switch (access) {
        case 'r': return (mode & S_IRUSR) ? 1 : 0;
        case 'w': return (mode & S_IWUSR) ? 1 : 0;
        case 'x': return (mode & S_IXUSR) ? 1 : 0;
        default:  return -1;
    }
}

int copy_file(unsigned cp_file, unsigned dstn_file, const char* destination_file, struct stat *sb, const unsigned max_len) {

    // аллоцирем буфер для чтения
    char* buf = (char*) calloc(max_len, sizeof(char));
    assert(buf != NULL);

    long long copy_file_size = (long long) sb->st_size;

    // копируем информацию
    while(copy_file_size > 0) {

        ssize_t read_symb_amount = read(cp_file, buf, max_len);

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
    struct utimbuf file_time_buf;

    // кладем в стуктуру время доступа и модификации исходного файла
    file_time_buf.actime  = sb->st_atime;
    file_time_buf.modtime = sb->st_mtime;

    // присваиваем новому файлу параметры времени исходника
    if (utime(destination_file, &file_time_buf) != 0) {
        perror("Error, impossible to assign the values of access time and modefite time.");
        rm_file(destination_file);
        close(dstn_file);
        return RESULT_BAD_COPY_TIME;
    }

    //-----------------------------------------------------------------------------------------

    // Copying User Id and Group Id
    if(fchown(dstn_file, sb->st_uid, sb->st_gid) < 0) 
    {
        perror("Failure while copying UID and GID");
        return 8;
    }

    // Copying access permissions
    if(fchmod(dstn_file, sb->st_mode) < 0) 
    {
        perror("Failure while copying access permissions");
        return 8;
    }
    // --------------------------------------------------------------------------------------------
    
    free(buf);

    return RESULT_OK;
}

char file_mode(unsigned mode) {

    switch (mode & S_IFMT) {
        case S_IFBLK:  return 'b';   break;
        case S_IFCHR:  return 'c';   break;
        case S_IFDIR:  return 'd';   break;
        case S_IFIFO:  return 'f';   break;
        case S_IFLNK:  return 'l';   break;
        case S_IFREG:  return 'r';   break;
        case S_IFSOCK: return 's';   break;
        default:       return '?';   break;
    }

    return '?';

}

char dtype_char(unsigned char dtype) {

    switch (dtype) {
        case DT_BLK :     return 'b'; break; 
        case DT_CHR:      return 'c'; break;
        case DT_DIR:      return 'd'; break;
        case DT_FIFO:     return 'f'; break;
        case DT_LNK:      return 'l'; break;
        case DT_REG:      return 'r'; break;
        case DT_SOCK:     return 's'; break;
        case DT_UNKNOWN:  return '?'; break;
    }

    return '?';
}

// (оформление) функция печатает отступы в соответсвии с вложенностью файла
void PrintLevel(int level) {
    while (level-- > 0) {
        printf("-");
    }
}

// вывод дериктории в консоль
int PrintDir(char* buf, char entry_type, char* file_name, int level) {
    PrintLevel(level);
    fprintf(stdout, " | %s %c %s\n", buf, entry_type, file_name);
    return RESULT_OK;
}

// функция проверяет, можно ли безопасно читать данный каталог
int check_dir(char entry_type, char* dr_name, mode_t mode) {
    // проверяем, что имеем дело с каталогом
    if (entry_type != 'd') {
        return NOT_DIR;
    }
    // проверяем право на чтение
    if (!(mode & S_IRUSR)) {
        return RESULT_BAD_READ;
    }

    // проверяем, что это не текущий и не родительский каталог
    if ((strcmp(dr_name, "..")) && (strcmp(dr_name, "."))) {
        #ifdef DOT_GIT_VEIW
            return SUITABLE_DIR;
        #else
            if (strcmp(dr_name, ".git")) {
                return SUITABLE_DIR;
            }
        #endif
    }

    return -1;
}

int RunDir(int sys_dir_fd, int level) {

    // получаем C-шный файловый дескриптор из системного
    DIR* dir_fd = fdopendir(sys_dir_fd);

    if (dir_fd == NULL) {
        perror("Bad dir file descriptor");
        return RESULT_ERR;
    }

    struct dirent* entry;
    struct stat sb;

    while ((entry = readdir(dir_fd)) != NULL) {

        char entry_type = dtype_char(entry->d_type);
        assert((fstatat(sys_dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW)) == 0);

        if (entry_type == '?') {
            entry_type = file_mode(sb.st_mode);
        }

        char* buf = (char*) calloc(9, sizeof(char));
        assert(buf != NULL);
        get_access(sb.st_mode, buf);
        PrintDir(buf, entry_type, entry->d_name, level);
        free(buf);

        if (check_dir(entry_type, entry->d_name, sb.st_mode) == SUITABLE_DIR) {
            int new_dir_fd = openat(sys_dir_fd, entry->d_name, O_RDONLY);

            level+=4;
            RunDir(new_dir_fd, level);
            level-=4;
            close(new_dir_fd);
        }
    }
    
    assert(errno == 0);
    return RESULT_OK;
}

int CopyLink(const char* copy_link, const char* dst_link) {
    char* buf = (char*) calloc(PATH_MAX, sizeof(char));

    if (readlink(copy_link, buf, PATH_MAX) < 0) {
        perror("Failed to read link");
        free(buf);
        return -1;
    }

    if (symlink(buf, dst_link) < 0) {
        perror("Failed to create a  new link");
        free(buf);
        return -1;
    }
  
    return 0;
}

int copy_reg_file(unsigned cp_file, unsigned dstn_file, const char* destination_file, struct stat *sb, const unsigned max_len) {

    // аллоцирем буфер для чтения
    char* buf = (char*) calloc(max_len, sizeof(char));
    assert(buf != NULL);

    long long copy_file_size = (long long) sb->st_size;

    // копируем информацию
    while(copy_file_size > 0) {

        ssize_t read_symb_amount = read(cp_file, buf, max_len);

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

    free(buf);

    return RESULT_OK;
}