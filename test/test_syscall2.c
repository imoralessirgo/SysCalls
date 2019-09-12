#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include "processinfo.h"


// These values MUST match the syscall_32.tbl modifications:
#define __NR_cs3013_syscall2 378

struct processinfo info;


long testCall2 (struct processinfo *info) {
        return (long) syscall(__NR_cs3013_syscall2);
}

int main () {
	struct processinfo test_info;
        printf("The return values of the system calls are:\n");
        //printf("\tcs3013_syscall2: %ld\n", testCall2(&test_info));
        return 0;
}
