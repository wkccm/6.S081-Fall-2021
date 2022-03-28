#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

int main(int argc, char *argv[]){
    if(argc!=1){
        print("sysinfo need no arg\n");
        exit(1);
    }
    struct sysinfo info;
    sysinfo(&info);
    printf("free space:%d, used process num:%d\n",info.freemem, info.nproc);
    exit(0);
}