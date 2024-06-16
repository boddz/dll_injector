CC   	 := clang
STANDARD := c99
FLAGS 	 := -Wall -Wextra
INCLUDE  := src
BUILD 	 := build
OBJECTS  := ${BUILD}/autoproc.o ${BUILD}/utils.o
EXE      := dllinject.exe

all: main.c

clean:
	-rmdir ${BUILD} -Force -Recurse /s /q

initbuild:
	mkdir ${BUILD}

main.c: clean initbuild autoproc.o utils.o
	${CC} -std=${STANDARD} ${FLAGS} -I${INCLUDE} ${OBJECTS} main.c -o ${BUILD}/${EXE}

run: all
	.\${BUILD}/${EXE}

autoproc.o: ${INCLUDE}/autoproc.c
	${CC} -std=${STANDARD} ${FLAGS} -c ${INCLUDE}/autoproc.c -o ${BUILD}/autoproc.o

utils.o: ${INCLUDE}/utils.c
	${CC} -std=${STANDARD} ${FLAGS} -c ${INCLUDE}/utils.c -o ${BUILD}/utils.o
