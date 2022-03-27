#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXN 1024

int main(int argc, char *argv[]){
    if(argc<2){
        printf("insufficient argument\n");
        exit(1);
    }
    char *argvs[MAXARG];
    int index=0;
    for(int i=1;i<argc;i++){
        argvs[index++]=argv[i];
    }
    char buf[MAXN];
    char block[MAXN];
    char* p=buf;
    int n;
    int m=0;
    while((n=read(0,block,sizeof(buf)))>0){
        for(int i=0;i<n;i++){
            if(block[i]=='\n'){
                buf[m]=0;
                m=0;
                argvs[index++]=p;
                p=buf;
                argvs[index]=0;
                index=argc-1;
                if(fork()==0){
                    exec(argv[1],argvs);
                }
                wait(0);
            }
            else if(block[i]==' '){
                buf[m++]=0;
                argvs[index++]=p;
                p=&buf[m];
            }
            else{
                buf[m++]=block[i];
            }
        }
    }
    exit(0);
}


