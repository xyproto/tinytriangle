x=/tmp/c;dd bs=1 skip=78 if=$0 of=$x.xz 2>$x;xz -df $x.xz;chmod +x $x;$x;exit
