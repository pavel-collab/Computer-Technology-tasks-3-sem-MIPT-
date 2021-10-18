enum {
    RESULT_OK = 0,
    RESULT_BAD_ARG,
    RESULT_BAD_FILE_TYPE,
    RESULT_OPEN_FAILED, 
    RESULT_BAD_READ,
    RESULT_BAD_WRITE,
    RESULT_BAD_CLOSE,
    RESULT_BAD_FILE_DELETE,
    RESULT_BAD_COPY_TIME,
    RESULT_BAD_READLINK,
    RESULT_ERR,
    NOT_DIR,
    SUITABLE_DIR
};

char* get_UTC_time(char* str, const time_t* s_time, unsigned buf_size);

const char* file_type(mode_t mode);

void get_access(mode_t st_mode, char* buf);

ssize_t writeall(int fd, const void *buf, size_t count);

int rm_file(const char* filename);

int check_user_access(const char* file_name, const char access, mode_t mode);

// функции передается:
//                     файловый дескриптор на копируемый файл
//                     файловый дескриптор на файл назначения
//                     имя файла назначения
//                     объект структуры stat с ифнормацией о файле
//                     размер блока чтения
int copy_file(unsigned cp_file, unsigned dstn_file, const char* destination_file, struct stat *sb, const unsigned max_len);

char file_mode(unsigned mode);

char dtype_char(unsigned char dtype);

void PrintLevel(int level);

int PrintDir(char* buf, char entry_type, char* file_name, int level);

int check_dir(char entry_type, char* dr_name, mode_t mode);

int RunDir(int sys_dir_fd, int level);

int CopyLink(const char* copy_link, const char* dst_link);

int copy_reg_file(unsigned cp_file, unsigned dstn_file, const char* destination_file, struct stat *sb, const unsigned max_len);