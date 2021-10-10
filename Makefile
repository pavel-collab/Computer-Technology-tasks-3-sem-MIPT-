# компилятор
C=gcc
# флаги компиляции
C_FLAGS=-Werror -Wall -Wextra
# флаги оптимизации
C_OPTIMIZE_FLAGS=-O2 -flto -fomit-frame-pointer



all: 1stat 2.1write 2.2dprintf 3.1cpregf 3.2pread 3copy 6.1readdir 6.2chdir 6.2version2 6.3rundir

1stat: 1stat.c flib.c
	$(C) -o 1stat $(C_FLAGS) 1stat.c flib.c
2.1write: 2.1write.c flib.c
	$(C) -o 2.1write $(C_FLAGS) 2.1write.c flib.c 
2.2dprintf: 2.2dprintf.c flib.c
	$(C) -o 2.2dprintf $(C_FLAGS) 2.2dprintf.c flib.c 
3.1cpregf: 3.1cpregf.c flib.c
	$(C) -o 3.1cpregf $(C_FLAGS) 3.1cpregf.c flib.c 
3.2pread: 1stat.c flib.c
	$(C) -o 3.2pread $(C_FLAGS) 3.2pread.c flib.c 
3copy: 3copy.c flib.c
	$(C) -o 3copy $(C_FLAGS) 3copy.c flib.c 
6.1readdir: 6.1readdir.c flib.c
	$(C) -o 6.1readdir $(C_FLAGS) 6.1readdir.c flib.c
6.2chdir: 6.2chdir.c flib.c
	$(C) -o 6.2chdir $(C_FLAGS) 6.2chdir.c flib.c
6.2version2: 6.2version2.c flib.c
	$(C) -o 6.2version2 $(C_FLAGS) 6.2version2.c flib.c 
6.3rundir: 6.3rundir.c flib.c
	$(C) -o 6.3rundir $(C_FLAGS) 6.3rundir.c flib.c

clear:
	rm 1stat 2.1write 2.2dprintf 3.1cpregf 3.2pread 3copy 6.1readdir 6.2chdir 6.2version2 6.3rundir