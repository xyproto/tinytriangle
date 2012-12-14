NAME = tinytriangle

# -fshort-double makes OpenGL stop working for some reason
# try both -Os and -O1 to see what produces the smallest result
CFLAGS = -Os -ffast-math -fno-inline -fomit-frame-pointer -nostdlib #-fpeephole2  -fexpensive-optimizations
SOURCES = util.s main.c
OBJS = util.o main.o
HEADERSCRIPT = header.sh
CC = clang #gcc

UNAMEM := $(shell uname -m)
ifeq ($(UNAMEM),x86_64)
    LDLINUX = /lib/ld-linux-x86-64.so.2
else
    LDLINUX = /lib/ld-linux.so.2
endif

all: m32on64

m32on64: ${SOURCES}
	${CC} -m32 ${CFLAGS} -c ${SOURCES}
	ld -melf_i386 -dynamic-linker /lib/ld-linux.so.2 ${OBJS} /usr/lib32/libSDL.so /usr/lib32/libGL.so /usr/lib32/libpthread.so /usr/lib32/libc.so /usr/lib32/libasound.so -o ${NAME}.elf
	@make ${NAME}

native: ${SOURCES}
	${CC} ${CFLAGS} -c ${SOURCES}
	ld -dynamic-linker ${LDLINUX} ${OBJS} /usr/lib/libSDL.so /usr/lib/libGL.so /usr/lib/libpthread.so /lib/libc.so.6 /usr/lib/libasound.so -o ${NAME}.elf
	@make ${NAME}

${NAME}: ${NAME}.elf
	strip -s -R .comment -R .gnu.version ${NAME}.elf
	sstrip ${NAME}.elf
	cat ${HEADERSCRIPT} > ${NAME}
	xz -zc9e ${NAME}.elf >> ${NAME}
	chmod -x ${NAME}.elf
	chmod +x ${NAME}

clean:
	rm -f ${NAME}.elf ${NAME} ${OBJS} *~
