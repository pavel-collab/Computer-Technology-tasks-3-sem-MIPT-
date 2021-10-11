# компилятор
C=gcc
# флаги компиляции
C_FLAGS=-Werror -Wall -Wextra -Wnarrowing
# флаги оптимизации
C_OPTIMIZE_FLAGS=-O2 -flto -fomit-frame-pointer

SRC_PATH_TASK1=1_stat/main.c
SRC_PATH_TASK2_1=2.1_write/main.c
SRC_PATH_TASK2_2=2.2_dprintf/main.c
SRC_PATH_TASK3=3_copy/main.c
SRC_PATH_TASK3_1=3.1_cp_reg_files/main.c
SRC_PATH_TASK3_2=3.2_pread/main.c
SRC_PATH_TASK6_1=6.1_readdir/main.c
SRC_PATH_TASK6_2=6.2_chdir/main.c
SRC_PATH_TASK6_2VER2=6.2_chdir/version2.c
SRC_PATH_TASK6_3=6.3_rundir/main.c

all: 1_stat 2.1_write 2.2_dprintf 3_copy 3.1_cp_reg_files 3.2_pread 6.1_readdir 6.2_chdir 6.2_version2 6.3_rundir

1_stat: ${SRC_PATH_TASK1} flib.c
	$(C) -o out ${SRC_PATH_TASK1} flib.c
2.1_write: ${SRC_PATH_TASK2_1} flib.c
	$(C) -o out ${SRC_PATH_TASK2_1} flib.c
2.2_dprintf: ${SRC_PATH_TASK2_2} flib.c
	$(C) -o out ${SRC_PATH_TASK2_2} flib.c
3_copy: ${SRC_PATH_TASK3} flib.c
	$(C) -o out ${SRC_PATH_TASK3} flib.c
3.1_cp_reg_files: ${SRC_PATH_TASK3_1} flib.c
	$(C) -o out ${SRC_PATH_TASK3_1} flib.c
3.2_pread: ${SRC_PATH_TASK3_2} flib.c
	$(C) -o out ${SRC_PATH_TASK3_2} flib.c
6.1_readdir: ${SRC_PATH_TASK6_1} flib.c
	$(C) -o out ${SRC_PATH_TASK6_1} flib.c
6.2_chdir: ${SRC_PATH_TASK6_2} flib.c
	$(C) -o out ${SRC_PATH_TASK6_2} flib.c
6.2_version2: ${SRC_PATH_TASK6_2VER2} flib.c
	$(C) -o out ${SRC_PATH_TASK6_2VER2} flib.c
6.3_rundir: ${SRC_PATH_TASK6_3} flib.c
	$(C) -o out ${SRC_PATH_TASK6_3} flib.c


clear:
	rm out