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

run.c: clean initbuild
	${CC} -std=${STANDARD} ${FLAGS} run.c -o ${BUILD}/autorun.exe

main.c: clean initbuild autoproc.o utils.o run.c
	${CC} -std=${STANDARD} ${FLAGS} -I${INCLUDE} ${OBJECTS} main.c -o ${BUILD}/${EXE}

run: all
	.\${BUILD}/${EXE}

autoproc.o: ${INCLUDE}/autoproc.c
	${CC} -std=${STANDARD} ${FLAGS} -c ${INCLUDE}/autoproc.c -o ${BUILD}/autoproc.o

utils.o: ${INCLUDE}/utils.c
	${CC} -std=${STANDARD} ${FLAGS} -c ${INCLUDE}/utils.c -o ${BUILD}/utils.o
