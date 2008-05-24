#include "syscall.h"
#include "stdio.h"

int main() {
    int cpid, exitstatus, exitcode;
    printf("jointest running!\n");
    cpid = exec("jointest2.coff",0,0);
    exitstatus = -99;
    exitcode = join(cpid, &exitstatus);
    printf("exitstatus = %d, exitcode = %d\n", exitstatus, exitcode);
    
}