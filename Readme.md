# MIPT 3 sem
Task from working in 3sem.
## Task 1 Stat.

Реализация аналога пользовательской команды stat.  
Compile with gcc -Wall -Wextra -o 1_stat 1_stat.c

### STAT, LSTAT, FSTAT

<span style="text-decoration: underline">stat</span> 
возвращает информацию о файле file_name и заполняет буфер buf. 

<span style="text-decoration: underline">lstat</span> 
идентична stat, но в случае символьных сылок она возвращает информацию о самой ссылке, а не о файле, на который она указывает.

<span style="text-decoration: underline">fstat</span> 
идентична stat, только возвращается информация об открытом файле, на который указывает filedes (возвращаемый open(2)), а не о file_name.

### ШПАРГАЛКА ПО STRUCT STAT

st_dev      устройство  
st_ino      inode  
st_mode     режим доступа  
st_nlink    количество жестких ссылок  
st_uid      ID пользователя-владельца  
st_gid      ID группы-владельца  
st_rdev     тип устройства  
st_size     общий размер в байтах  
st_blksize  размер блока ввода-вывода  
st_blocks   количество выделенных блоков  
st_atime    время последнего доступа  
st_mtime    время последней модификации  
st_ctime    время последнего изменения  

### ШПАРГАЛКА ПО ST_MODE

S_ISUID     04000   set-user-ID bit  
S_ISGID     02000   set-group-ID bit  
S_ISVTX     01000   sticky bit  

S_IRWXU     00700   owner has read, write, and execute permission  
S_IRUSR     00400   owner has read permission  
S_IWUSR     00200   owner has write permission  
S_IXUSR     00100   owner has execute permission  

S_IRWXG     00070   group has read, write, and execute permission  
S_IRGRP     00040   group has read permission  
S_IWGRP     00020   group has write permission  
S_IXGRP     00010   group has execute permission  

S_IRWXO     00007   others (not in group) have read, write, and execute permission  
S_IROTH     00004   others have read permission  
S_IWOTH     00002   others have write permission  
S_IXOTH     00001   others have execute permission  

___

Так же смотри man 2 stat, man 2 inode, man 2 statx.

## Task 2.1 write.

as soon as possible

## Task 2.2 dprintf.

as soon as possible

## Task 3.1 the copy of regular files.

as soon as possible

## Task 3.3

pread, prwite используются заместо read + lseek, чтобы не делать 2 системных вызова для одного действия

огласно POSIX требуется, чтобы открытие файла с флагом O_APPEND не влияло на расположение, по которому pwrite() записывает данные. Однако в Linux, если файл открывается с флагом O_APPEND, pwrite() добавляет данные в конец файла, независимо от значения offset.