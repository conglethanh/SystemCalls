#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>

asmlinkage int sys_pidtoname(int pid, char *buf, int len){
    /*tasklist struct to use*/
    struct task_struct *task;
    
 printk("syscall_pidtoname: input PID = %d\n",pid);
    for_each_process(task){
        int tempPID = (int)task_pid_nr(task);
        if(pid==tempPID){
                        char tmp[len];
			strncpy(tmp,task->comm,len);
			tmp[len-1]='\0';
			copy_to_user(buf,tmp,len);
			

			printk("PID = %d has process name: %s\n",pid, task->comm);
			//since strlen returns the length of a string without '\0', but len we have already included '\0'
			if (len -1 > strlen(task->comm)){
				return 0;
				
			}
			else if (len -1 < strlen(task->comm))
				//return len full process name without '\0'
				return strlen(task->comm);
			else
				return len-1;
        }
    }
    return -1;
}
