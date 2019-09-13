#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include "processinfo.h"
#include <sys/time.h>
#include <sys/resource.h>
// These values MUST match the syscall_32.tbl modifications:
#define __NR_cs3013_syscall2 378



long testCall2 (struct processinfo *info) {
        return (long) syscall(__NR_cs3013_syscall2,info);
}


void printProcessInfo(struct processinfo* test_info){
		printf("The current state of the process: %ld \n",test_info->state);
                printf("The Process ID is: %d \n",test_info->pid);
                printf("The Parent PID is: %d\n", test_info->parent_pid);
                printf("The Process ID of youngest child is: %d\n",test_info->youngest_child);
                printf("The Process ID of younger Sibling: %d\n", test_info->younger_sibling);
                printf("The Process ID of older sibling: %d\n", test_info->older_sibling);
                printf("The User ID of process owner: %d\n", test_info->uid);
                printf("Process start time since boot: %lld\n", test_info->start_time);
                printf("CPU time in user mode: %lld\n",test_info->user_time);
                printf("CPU time in system  mode: %lld\n",test_info->sys_time);
                printf("User time of children: %lld\n",test_info->cutime);
                printf("System time of children: %lld\n", test_info->cstime);
}


int main () {

	
	struct processinfo test_info;
        printf("The return values of the system calls are:\n");

	if(fork()==0){
		printf("\t(CHILD) cs3013_syscall2: %ld\n", testCall2(&test_info));
		
		printProcessInfo(&test_info);

        	return 0;

	}else{	
        	printf("\t(PARENT) cs3013_syscall2: %ld\n", testCall2(&test_info));
		wait(0); //wait on child process
	
		printf("____ PARENT PROCESS INFO ____\n");
		
		printProcessInfo(&test_info);
			
        	return 0;
	}

}
