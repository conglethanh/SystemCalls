
#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <asm/cacheflush.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Thanh Cong - Nguyen Quoc Vuong");

void **system_call_table_addr;
/*my custom syscall that takes process name*/
asmlinkage int (*original_open) (const char* pathname, int flags);
/*hook*/
asmlinkage int hook_open_syscall(const char* pathname, int flags) {
	char name[64];
	copy_from_user(name,pathname,64);
	printk("Process %s is opening file: %s\n",current->comm,name);
	return (*original_open)(pathname,flags);
}
/*Make page writeable*/
int make_rw(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if(pte->pte &~_PAGE_RW){
        pte->pte |=_PAGE_RW;
    }
    return 0;
}
/* Make the page write protected */
int make_ro(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~_PAGE_RW;
    return 0;
}
static int __init entry_point(void){
    printk(KERN_INFO "Hook open syscall loaded successfully..\n");
    /*MY sys_call_table address*/
    system_call_table_addr = (void*)0xffffffff81a00180;
    /* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
    original_open = system_call_table_addr[__NR_open];
    /*Disable page protection*/
    make_rw((unsigned long)system_call_table_addr);
    /*Change syscall to our syscall function*/
    system_call_table_addr[__NR_open] = hook_open_syscall;
    return 0;
}
static int __exit exit_point(void){
        printk(KERN_INFO "Unloaded Hook open syscall successfully\n");
    /*Restore original system call */
    system_call_table_addr[__NR_open] = original_open;
    /*Renable page protection*/
    make_ro((unsigned long)system_call_table_addr);
    return 0;
}
module_init(entry_point);
module_exit(exit_point);

