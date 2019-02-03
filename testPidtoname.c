#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(){
    int pid,len;
    puts("Enter PID to find: ");
    scanf("%d",&pid);
    char* buf = (char*)malloc(64);
    puts("Enter len: ");
    scanf("%d",&len);
    int n = syscall(351, pid, buf,len); 

    printf("Process name: %s\n",buf);
    printf("System call returned n: %d\n", n);
    return 0;
}

