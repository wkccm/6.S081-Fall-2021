#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if(2!=argc){
        printf("Need 1 argument for function sleep\n");
        exit(1);
    }
    int SleepLength=atoi(argv[1]);
    printf("Sleep for a while\n");
    sleep(SleepLength);
    exit(0);
}