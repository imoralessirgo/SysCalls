#ifndef __PROCESSINFO_H
#define __PROCESSINFO_H


struct processinfo{
	long state;
	pid_t pid;
	pid_t parent_pid;
	pid_t youngest_child;
        pid_t younger_sibling;
        pid_t older_sibling;
        uid_t uid;
	long long start_time;
	long long user_time;
	long long sys_time;
	long long cutime;
	long long cstime;
};


#endif
