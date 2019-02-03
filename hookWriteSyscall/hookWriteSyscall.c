#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <asm/cacheflush.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Thanh Cong - Nguyen Quoc Vuong");

void **system_call_table_addr;
/*my custom syscall that takes process name*/
asmlinkage int (*original_write) (int fd, const void *buf, size_t count);
/*hook*/
asmlinkage int hook_write_syscall(int fd, const void *buf, size_t count) {
	
	char *tmp;
	char *pathname;
	struct file *file;
	struct path *path;
	struct files_struct *files = current->files;
	
	spin_lock(&files->file_lock);
	file = fcheck_files(files, fd);
	if (!file) {
	    spin_unlock(&files->file_lock);
	    return -ENOENT;
	}

	path = &file->f_path;
	path_get(path);
	spin_unlock(&files->file_lock);

	tmp = (char *)__get_free_page(GFP_KERNEL);

	if (!tmp) {
	    path_put(path);
	    return -ENOMEM;
	}

	pathname = d_path(path, tmp, PAGE_SIZE);
	path_put(path);

	if (IS_ERR(pathname)) {
	    free_page((unsigned long)tmp);
	    return PTR_ERR(pathname);
	}

	printk("Process: %s is writting file: %s with number of bytes: %zu\n",current->comm,pathname,count);

	free_page((unsigned long)tmp);

	return (*original_write)(fd,buf,count);

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
    printk(KERN_INFO "Hook write syscall loaded successfully..\n");
    /*MY sys_call_table address*/
    system_call_table_addr = (void*)0xffffffff81a00180;
    /* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
    original_write = system_call_table_addr[__NR_write];
    /*Disable page protection*/
    make_rw((unsigned long)system_call_table_addr);
    /*Change syscall to our syscall function*/
    system_call_table_addr[__NR_write] = hook_write_syscall;
    return 0;
}
static int __exit exit_point(void){
        printk(KERN_INFO "Unloaded Hook write syscall successfully\n");
    /*Restore original system call */
    system_call_table_addr[__NR_write] = original_write;
    /*Renable page protection*/
    make_ro((unsigned long)system_call_table_addr);
    return 0;
}
module_init(entry_point);
module_exit(exit_point);

