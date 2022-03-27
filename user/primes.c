#include "kernel/types.h"
#include "user/user.h"

void mapping(int n,int pd[]){
    close(n);
    dup(pd[n]);
    close(pd[0]);
    close(pd[1]);
}

void primes(){
    int pre,next;
    int fd[2];
    if(read(0,&pre,sizeof(int))){
        printf("prime %d\n",pre);
        pipe(fd);
        if(fork()==0){
            mapping(1,fd);
            while (read(0,&next,sizeof(int)))
            {
                if(next%pre!=0){
                    write(1,&next,sizeof(int));
                }
            }
            
        }
        else{
            wait(0);
            mapping(0,fd);
            primes();
        }
    }
}

int main(){
    int fd[2];
    pipe(fd);
    if(fork()==0){
        mapping(1,fd);
        for(int i=2;i<36;i++){
            write(1,&i,sizeof(int));
        }
    }
    else{
        wait(0);
        mapping(0,fd);
        primes();
    }
    exit(0);
}