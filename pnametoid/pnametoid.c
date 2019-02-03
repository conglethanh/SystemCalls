#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>

asmlinkage int sys_pnametoid(char* name){
    /*tasklist struct to use*/
    printk("Pnametoid syscalls start!!!");
    struct task_struct *task;
    char kname[64];
    copy_from_user(kname,name,64);
    printk("name(User)/kname(Kernel) = %s\n",kname);
    
    for_each_process(task){
        /*compares the current process name (defined in task->comm) to the passed in name*/
        if(strcmp(task->comm, kname) == 0){
		printk("PID for name: %d\n",(int)task_pid_nr(task));
         	return (int)task_pid_nr(task);
        }
    }
    printk("PID for name: %d\n",-1);
    return -1;
}

