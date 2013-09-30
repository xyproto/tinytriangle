Differences for the Red Hat edition:

* Tries to depend on X11 and GL instead of SDL
* Tries to depend on bzip2 instead of xz

---

Noisy Triangle
A skeleton for 4k intros on Linux

Uses ALSA for audio and SDL+OpenGL for graphics.
Executable ends up at 1422 bytes at the time of writing.

Alexander Rødseth <rodseth@gmail.com>

Resources:
    http://www.int21.de/linux4k/
    http://ftp.kameli.net/pub/fit/misc/presis_asm06.pdf
    http://glprogramming.com/red/chapter07.html
    http://www.alsa-project.org/alsa-doc/alsa-lib/pcm.html

Building the 32-bit version on x86_64 for x86_64:
    To build with make:
	make m32on64
    Requirements:
        elfkicker (only used in the build process)
        xz
        lib32-sdl
        lib32-glibc
        lib32-nvidia-utils (or lib32-libgl)
        lib32-alsa-lib

Building the native version (32-bit for i686, 64-bit for x86_64):
    To build with make:
	make native
    Requirements:
        elfkicker (only used in the build process)
        xz
        sdl
        glibc
        nvidia-utils (or libgl)
        alsa-lib

About unpackme.sh:
    x=/dev/shm/x
	prepares to write the uncompressed elf file to shared memory as "x"
    dd bs=1 skip=82 if=$0 of=$x.xz 2>$x
	dd reads from "if": $0, which is the file with the shellscript + compressed elf file combined
	and writes to "of": $x.xz, which is a file in shared memory (/dev/shm/x.xz)
	bs=1 and skip=82 sets byte size to 1 and skips the 82 first bytes (the shell code)
	2>$x gets rid of the output from dd by piping from stderr to $x (/dev/shm/x).
	The reason why $x is used instead of /dev/null is because it's shorter to write and
	since /dev/shm/x will be overwritten by the next command in any case.
    xz -df $x.xz
	extracts the compressed elf file in in /dev/shm/x.xz and writes the result
	to /dev/shm/x. It overwrites /dev/shm/x because of the "f" flag.
	-d is for decompress
    chmod +x $x
	makes the uncompressed ELF at /dev/shm/x executable
    $x
	runs the executable ELF at /dev/shm/x

Improvements? Comments? Send me an e-mail.
