#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include "processinfo.h"
#include <sys/time.h>
#include <sys/resource.h>
// These values MUST match the syscall_32.tbl modifications:
#define __NR_cs3013_syscall2 378

struct processinfo info;
struct rusage usage;
struct timezone tzone;

long testCall2 (struct processinfo *info) {
        return (long) syscall(__NR_cs3013_syscall2,info);
}

int main () {
	struct processinfo test_info;
        printf("The return values of the system calls are:\n");
	
        printf("\tcs3013_syscall2: %ld\n", testCall2(&test_info));
	
	getrusage(test_info.pid,&usage);
	double ucpu =0;
	double scpu =0;
	 ucpu = ((double) usage.ru_utime.tv_sec * (double)1000000) + ((double) usage.ru_utime.tv_usec);
	 scpu = ((double) usage.ru_stime.tv_sec * (double)1000000) + ((double) usage.ru_stime.tv_usec);
	
	
	printf("The current state of the process: %d \n",test_info.state);
	printf("The Process PID is: %d \n",test_info.pid);
	printf("The Parent PID is: %d\n", test_info.parent_pid);	
	printf("CPU time in user mode: %f\n",((double) usage.ru_utime.tv_sec * (double)1000000) + ((double) usage.ru_utime.tv_usec));
	printf("CPU time in system  mode: %f\n",((double) usage.ru_stime.tv_sec * (double)1000000) + ((double) usage.ru_stime.tv_usec));	
        return 0;
}
