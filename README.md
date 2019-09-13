Joseph Cybul 
Isabel Morales 

The structure of the file is the main folder contains the mymodule.c and its makefile and inside the main folder there is another
folder called test, inside test test_syscall2.c and its makefile


Provided files:
mymodule.c, has the source code for both part 1 and 2. and make file to build 
test_syscall2.c, has source code to test the module, and the respective prints. and make file to build
syslog text files to show our tests.

To run:
1. run make in the main folder(project 2).
2. sudo rmmod mymodule.ko
3. sudo insmod mymodule.ko 
4. enter the test foler and run make there 
5. then run the executable  ./test_sycall2 
6. open the syslog to see prints in the kernel.

Note: ignore child PID in part syslog, defined after the print K.
