# компилятор
C=gcc
# флаги компиляции
C_FLAGS=-Werror -Wall -Wextra -Wnarrowing -Wwrite-strings -Wcast-qual -Wundef -Wstrict-prototypes -Wbad-function-cast
# флаги оптимизации
C_OPTIMIZE_FLAGS=-O2 -flto -fomit-frame-pointer
# объектники
OBJECT_FILES=main.o flib.o

all: task1 task2.1 task2.2 task3.1 task3.2 task3 task6.1 task6.2 task6.3

task1:
task2.1: 
task2.2: 
task3.1: 
task3.2:
task3:
task6.1:
task6.2: 
task6.3:

clear:
	rm *.o out