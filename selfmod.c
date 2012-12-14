#include <sys/mman.h>

unsigned long *addr = (unsigned long*)get_address_of_instruction_pointer();

void main() {
  int length = 4096;   /* size of a page */
  if (mprotect(addr, length, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
      /* current code page is now writable and code from it is allowed for execution */
  }
}
