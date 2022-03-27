#include "kernel/types.h"
#include "user/user.h"

int main(){
    //pipe函数，read p[0], write p[1]
    int p1[2], p2[2];
    char buf[]={'x'};
    //type.h中long最长, 为uint64
    int length=strlen(buf);
    pipe(p1);
    pipe(p2);
    //fork(),子进程返回0，父进程返回PID，注意构思管道通信过程
    if(fork()==0){
        if(read(p1[0],buf,length)!=length){
            printf("Read from parent to child error!");
            exit(1);
        }
        printf("%d: received ping\n",getpid());
        if(write(p2[1],buf,length)!=length){
            printf("Write from p2 to p1 error!");
            exit(1);
        }
        
    }
    else{
        if(write(p1[1],buf,length)!=length){
            printf("Write from parent to child error!");
            exit(1);
        }
        wait(0);
        if(read(p2[0],buf,length)!=length){
            printf("Read from child to parent error!");
            exit(1);
        }
        printf("%d: received pong\n",getpid());
        
    }
    exit(0);
}