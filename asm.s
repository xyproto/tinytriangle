/* ---- int add(int a, int b) ---- */
    .globl add
add:
    movl    4(%esp), %eax
    addl    8(%esp), %eax
    ret

/* ---- void quit() ---- */
    .globl quit
quit:
    movl $1, %eax
    xor %ebx, %ebx
    int $128
    /* no ret needed */
