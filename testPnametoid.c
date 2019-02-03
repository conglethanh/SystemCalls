
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
int main(){
    char name[64];
    puts("Enter process to find");
    scanf("%s",name);
    strtok(name, "\n");
    int pid = syscall(350, name); 
    printf("System call returned PID: %d\n", pid);
    return 0;
}

