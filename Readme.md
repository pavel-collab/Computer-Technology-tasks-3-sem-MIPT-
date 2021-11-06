# MIPT 3 sem
Task from working in 3sem.

## Build
To build the project firstly you need clone the repository to your local computer.
```
git clone https://github.com/pavel-collab/Computer-Technology-tasks-3-sem-MIPT-/tree/main
```
Open the cloned directory and build the task that you need.
```
make <task name>
```
List of avaliable tasks:
* 1_stat 
* 2.1_write 
* 2.2_dprintf 
* 3_copy 
* 3.1_cp_reg_files 
* 3.2_pread 
* 6.1_readdir 
* 6.2_chdir 
* 6.2_version2 
* 6.3_rundir

After you will done it, in current directory you will see an executable file named "out".
Start the programm in comand line with suitable arguments according to the task you have built.
```
./out <arguments>
```

### ATTENTION
If you use command 
```
make
``` 
without any arguments the Makefile default builds the last task.

## Task 1 Stat.
Программа принимает на вход имя файла и выводит в консоль информацию о нем.
```
./out <filename>
```
### Example
```
./out Readme.md

File:                     Readme.md
ID of containing device:  [103,5]
File type:                regular file
access:                   664/rw-rw-r--
I-node number:            655844
Mode:                     100664 (octal)
Link count:               1
Ownership:                UID=1000   GID=1000
Preferred I/O block size: 4096 bytes
File size:                1556 bytes
Blocks allocated:         8
Last status change:       10/11/21 Monday 11:34:31 (UTC)
Last file access:         10/11/21 Monday 09:28:26 (UTC)
Last file modification:   10/11/21 Monday 11:34:31 (UTC)
```

## Task 2.1 write.
На вход программе подается имя файла и строка. Программа записывае заданную строку в указанный файл. 
Если указанного файла не существует, он будет создан.
```
./out <file name> <str>
```

### Example
```
./out output.txt Hello
```

## Task 2.2 dprintf.
На вход программе подается имя файла и строка. Программа записывае заданную строку в указанный файл. 
Если указанного файла не существует, он будет создан.
```
./out <file name> <str>
```

### Example
```
./out output.txt Hello
```

## Task 3.1 the copy of regular files.
Программа на вход получает обычный (regular) файл, который необходимо скопировать и имя файла-копии.
После этого программа скопирует содержимое указанного файла в новый файл.
```
./out <copying file> <destination file>
```

### Example
```
./out test.txt new.txt
```
test.txt -- копируемый файл
new.txt  -- новый файл с скопированным из test.txt содержимым

## Task 3.2
Программа на вход получает обычный (regular) файл, который необходимо скопировать, и имя файла-копии.
После этого программа скопирует содержимое указанного файла в новый файл, причем в новом файле блоки будут выведены в ОБРАТНОМ ПОРЯДКЕ относительно исходного файла.
```
./out <copying file> <destination file>
```

### Example
```
./out test.txt new.txt
```
test.txt -- копируемый файл
new.txt  -- новый файл с скопированным из test.txt содержимым

## Task 3
Программа на вход получает файл, который необходимо скопировать, и имя файла-копии.
Программа может копировать: обычные файлы, symlinks, fifo files, nods,а так же ПУСТЫЕ каталоги.
```
./out <copying file> <destination file>
```

### Example
```
./out test new
```
test -- копируемый файл
new  -- новый файл

## Task 6.1
Программа выводит в консоль список всех записей в текущей дериктории (включая скрытые файлы, родительский каталог и текущий каталог).
Для каждой записи будет указан тип и права доступа.
```
./out
```

### Example
```
./out

[rwxrwxr-x] d 6.1_readdir
[rwxrwxrwx] l mylink
[rwxrwxr-x] d .git
[rw-rw-r--] r .gitignore
[rwxrwxr-x] r out
[rw-rw-r--] r sundbox.c
[rwxrwxr-x] d 3.1_cp_reg_files
[rw-rw-r--] r test.txt
[rwxrwxr-x] d 1_stat
[rw-rw-r--] r test1.txt
[rwxrwxr-x] d 3_copy
[rwxrwxrwx] d ..
[rwxrwxr-x] d 6.2_chdir
[rwxrwxr-x] d 2.2_dprintf
[rw-rw-r--] r enum.h
[rwxrwxr-x] d 2.1_write
[rw-rw-r--] r Makefile
[rw-rw-r--] r flib.c
[rwxrwxr-x] d 6.3_rundir
[rwxrwxr-x] d .
[rw-rw-r--] r Readme.md
[rwxrwxr-x] d 3.2_pread
```

## Task 6.2
Программа принимает на вход АБСОЛЮТНЫЙ путь к каталогу и выводит в консоль список записей в нем (включая скрытые файлы, родительский каталог и текущий каталог).
Для каждой записи будет указан тип и права доступа.
Если не передавать программе путь к файлу, по умолчанию будет выведен список записей в текущем каталоге.
```
./out <absolute path>
```

В основной файле программы используется системный вызов chdir. Во второй версии переход в указанный каталог с помощью chdir не осуществляется.

## Task 6.3
Программа принимает на вход АБСОЛЮТНЫЙ путь к каталогу и с помощью рекурентного обхода дерева каталогов выводит в консоль все записи до самрнр нижнего уровня.
Если не передавать программе путь к файлу, по умолчанию будет выведен список записей в текущем каталоге.
```
./out <absolute path>
```

### Example
```
./out

 | rwxrwxr-x d 6.1_readdir
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rwxrwxrwx l mylink
 | rwxrwxr-x d .git
 | rw-rw-r-- r .gitignore
 | rwxrwxr-x r out
 | rw-rw-r-- r sundbox.c
 | rwxrwxr-x d 3.1_cp_reg_files
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rw-rw-r-- r test.txt
 | rwxrwxr-x d 1_stat
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rw-rw-r-- r test1.txt
 | rwxrwxr-x d 3_copy
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rwxrwxrwx d ..
 | rwxrwxr-x d 6.2_chdir
---- | rw-rw-r-- r version2.c
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rwxrwxr-x d 2.2_dprintf
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rw-rw-r-- r enum.h
 | rwxrwxr-x d 2.1_write
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rw-rw-r-- r Makefile
 | rw-rw-r-- r flib.c
 | rwxrwxr-x d 6.3_rundir
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
 | rwxrwxr-x d .
 | rw-rw-r-- r Readme.md
 | rwxrwxr-x d 3.2_pread
---- | rw-rw-r-- r main.c
---- | rwxrwxr-x d ..
---- | rwxrwxr-x d .
```


### TODO LIST

- [ ] task 13.1 -- exchange <waitpit> to <waitid> + output full information about proces (man waitid)
- [ ] how to save core_dump in current dir
- [X] изменить основную дерикторию и переписать Makefile
- [X] сделать библиотеку функций
- [X] проверить, что все работает
- [ ] написать комментарии для функций
- [X] написать нормальный Readme
- [ ] перевести комментарии на англ
- [ ] перевести Readme на англ