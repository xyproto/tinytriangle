NAME = tinytriangle

# -fshort-double makes OpenGL stop working for some reason
# try both -Os and -O1 to see what produces the smallest result
CFLAGS = -Os -ffast-math -fno-inline -fomit-frame-pointer -nostdlib -fpeephole2 -fexpensive-optimizations
SOURCES = util.s main.c
OBJS = util.o main.o
HEADERSCRIPT = header.sh
CC = gcc
DYNAMIC = -dynamic-linker

UNAMEM := $(shell uname -m)
ifeq ($(UNAMEM),x86_64)
    # If this can't be found on x86_64, then compiling for 64-bit won't work
    # Arch Linux
    #LDLINUX_NATIVE = /lib/ld-linux-x86-64.so.2
    # Old Red Hat
    LDLINUX_NATIVE = /lib64/ld-linux-x86-64.so.2
    LDLINUX = /lib/ld-linux.so.2
else
    LDLINUX_NATIVE = /lib/ld-linux.so.2
    LDLINUX = /lib/ld-linux.so.2
endif

PLATFORM := redhat
ifeq ($(PLATFORM),redhat)
  # Old Red Hat
  LIB32FILES = /usr/lib/libXxf86vm.so /usr/lib/libX11.so /usr/lib/libGL.so /usr/lib/libGLU.so.1 /usr/lib/libpthread.so /usr/lib/libc.so /lib/libasound.so.2 
  LIBFILES = /usr/lib64/libXxf86vm.so /usr/lib64/libX11.so /usr/lib64/libGL.so /usr/lib64/libGLU.so.1 /usr/lib64/libpthread.so /lib64/libc.so.6 /lib64/libasound.so.2 
  SSTRIP =
else
  # Arch Linux
  LIB32FILES = /usr/lib32/libXxf86vm.so /usr/lib32/libX11.so /usr/lib32/libGL.so /usr/lib32/libGLU.so /usr/lib32/libpthread.so /usr/lib32/libc.so /usr/lib32/libasound.so 
  LIBFILES = /usr/lib/libXxf86vm.so /usr/lib/libX11.so /usr/lib/libGL.so /usr/lib/libGLU.so /usr/lib/libpthread.so /lib/libc.so.6 /usr/lib/libasound.so 
  SSTRIP = sstrip ${NAME}.elf
endif

all: native

m32on64: ${SOURCES} ${LIB32FILES}
	${CC} -m32 ${CFLAGS} -c ${SOURCES}
	ld -melf_i386 ${DYNAMIC} ${LDLINUX} ${OBJS} ${LIB32FILES} -o ${NAME}.elf
	@make ${NAME}

native: ${SOURCES} ${LIBFILES}
	${CC} ${CFLAGS} -c ${SOURCES}
	ld ${DYNAMIC} ${LDLINUX_NATIVE} ${OBJS} ${LIBFILES}-o ${NAME}.elf
	@make ${NAME}

${NAME}: ${NAME}.elf
	strip -s -R .comment -R .gnu.version ${NAME}.elf
	${SSTRIP}
	cat ${HEADERSCRIPT} > ${NAME}
	bzip2 -zc9 ${NAME}.elf >> ${NAME}
	chmod -x ${NAME}.elf
	chmod +x ${NAME}

clean:
	rm -f ${NAME}.elf ${NAME} ${OBJS} *~
