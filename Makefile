# компилятор
C=gcc
# флаги компиляции
C_FLAGS=-Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast
# флаги оптимизации
C_OPTIMIZE_FLAGS=-O2 -flto -fomit-frame-pointer

SRC_PATH=1_stat/main.c

1stat: ${SRC_PATH} flib.c
	gcc -o out ${SRC_PATH} flib.c

clear:
	rm *.o out