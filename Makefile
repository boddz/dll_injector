CC   	 := clang
STANDARD := c99
FLAGS 	 := -Wall -Wextra
BUILD 	 := build
EXE      := dllinject.exe


all: main.c

clean:
	-rmdir ${BUILD} -Force -Recurse /s /q

main.c: clean
	mkdir ${BUILD}
	${CC} -std=${STANDARD} ${FLAGS} main.c -o ${BUILD}/${EXE}

run: all
	.\${BUILD}/${EXE}
