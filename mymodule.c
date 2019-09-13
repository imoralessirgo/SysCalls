#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/list.h>

unsigned long **sys_call_table;


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

// refs to original syscalls
//asmlinkage long (*ref_sys_cs3013_syscall1)(void);
asmlinkage long (*ref_sys_cs3013_syscall2)(struct processinfo *info);
//asmlinkage long (*ref_sys_open)(const char *filename,int flags, umode_t mode);
//asmlinkage long (*ref_sys_close)(unsigned int fd);

 

/*asmlinkage long new_sys_open(const char *filename,int flags,umode_t mode){
	unsigned int regular_user = 1000;
	unsigned int usr = current_uid().val;
	if(usr >= regular_user){
		printk(KERN_INFO "User %d is opening file: %s\n",usr,filename);
	}
	return ref_sys_open(filename,flags,mode);
}

asmlinkage long new_sys_close(unsigned int fd){
	unsigned int regular_user = 1000;
	unsigned int usr = current_uid().val;
	if(usr >= regular_user){
		printk(KERN_INFO "User %d is closig file descriptor: %d",usr,fd);
	}
	return ref_sys_close(fd);
}

asmlinkage long new_sys_cs3013_syscall1(void) {
    printk(KERN_INFO "\"'Hello world?!' More like 'Goodbye, world!' EXTERMINATE!\" -- Dalek");
    return 0;
}*/

asmlinkage long new_sys_cs3013_syscall2(struct processinfo *info){
	int flag = 0;
	struct task_struct *task = current;
	struct processinfo kinfo;
	

	// get state 
	kinfo.state = (pid_t) task->state;
	
	//get pids
	kinfo.pid = (pid_t) task->pid;
	kinfo.parent_pid = (pid_t) task->real_parent->pid;

	//child pid 
	if(!list_empty(&task->children)){ //check for children
		//get last element of list (youngest of the children) & extract pid
		kinfo.youngest_child = list_last_entry(&task->children,struct task_struct,children)->pid;
	}else{ kinfo.youngest_child = -1;} // if no children set pid to -1
	

	/* task_struct.sibling is a doubly linked list, we can acces previous and next values to obtain our processes sibling info */
	//younger sibling pid
	if(list_entry(&task->sibling.next,struct task_struct, sibling)->pid > kinfo.pid && list_entry(&task->sibling.next,struct task_struct, sibling)->pid != 0 ){ //check next element in list if any
		kinfo.younger_sibling = list_entry(&task->sibling.next,struct task_struct, sibling)->pid; 
	}else{  kinfo.younger_sibling = -1; } // no younger sibling set pid to -1
	
	//older sibling pid
	if(list_entry(&task->sibling.prev,struct task_struct, sibling)->pid < kinfo.pid && list_entry(&task->sibling.prev,struct task_struct, sibling)->pid != 0 ){ // check previous element in list if any
		kinfo.older_sibling = list_entry(&task->sibling.prev,struct task_struct, sibling)->pid;
	}else{ kinfo.older_sibling = -1; }// no older sibling set pid to -1
	


	/*DEBUG PRINTKS*/
	printk(KERN_INFO "parent id: %d\n", kinfo.parent_pid);
	printk(KERN_INFO "youngest child : %d\n", kinfo.youngest_child);	
	printk(KERN_INFO "younger sibling %d\n", kinfo.younger_sibling);
	printk(KERN_INFO "older sibling : %d\n", kinfo.older_sibling);
	printk(KERN_INFO "State: %d\n",kinfo.state);

	// set uid
	kinfo.uid = task->real_cred->uid.val;
	printk(KERN_INFO "uid: %d\n", kinfo.uid);	


	// set times
	kinfo.start_time = timespec_to_ns(&task->start_time);
	kinfo.user_time = cputime_to_usecs(&task->utime);
	kinfo.sys_time = cputime_to_usecs(&task->stime);
	// if no children val will be 0
	printk(KERN_INFO "real time:%ld",kinfo.start_time);
	kinfo.cutime = 0;
	kinfo.cstime = 0;

	//loop over all children processes 
	if(!list_empty(&task->children)){//if we have children
		struct list_head *HEAD;
		list_for_each(HEAD,&task->children){ //built in for each element in given list
			struct task_struct *child;
			child = list_entry(HEAD,struct task_struct,children);
			printk(KERN_INFO "hello: %d\n", child->pid);
			kinfo.cutime += cputime_to_usecs(&child->utime);
       			kinfo.cstime += cputime_to_usecs(&child->stime);
		}
	}else{printk(KERN_INFO "NOCHILDDDDD\n");}

	
	if(copy_to_user(info,&kinfo, sizeof kinfo )){
		return EFAULT; // Report copy_to_user failure 
	}			
	return flag;
}

static unsigned long **find_sys_call_table(void) {
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;
  
  while (offset < ULLONG_MAX) {
    sct = (unsigned long **)offset;

    if (sct[__NR_close] == (unsigned long *) sys_close) {
      printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                (unsigned long) sct);
      return sct;
    }
    
    offset += sizeof(void *);
  }
  
  return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
   */
  write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the 
   16th bit to re-enable write protection on the CPU.
  */
  write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
  if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work. 
       Cancel the module loading step. */
    return -1;
  }
  
  /* Store a copy of all the existing functions */
  //ref_sys_cs3013_syscall1 = (void *)sys_call_table[__NR_cs3013_syscall1];
  ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
  //ref_sys_open = (void *)sys_call_table[__NR_open];
  //ref_sys_close = (void *)sys_call_table[__NR_close];


  /* Replace the existing system calls */
  disable_page_protection();

  //sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)new_sys_cs3013_syscall1;
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
  //sys_call_table[__NR_open] = (unsigned long*)new_sys_open;
  //sys_call_table[__NR_close] = (unsigned long*)new_sys_close;  

  enable_page_protection();
  
  /* And indicate the load was successful */
  printk(KERN_INFO "Loaded interceptors!");

  return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
  if(!sys_call_table)
    return;
  
  /* Revert all system calls to what they were before we began. */
  disable_page_protection();
  //sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)ref_sys_cs3013_syscall1;
  sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
  //sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
  //sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
  enable_page_protection();

  printk(KERN_INFO "Unloaded interceptors!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
