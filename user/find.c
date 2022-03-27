#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char* file){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        printf("find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    if(st.type!=T_DIR){
        printf("find: %s is not a directory\n", path);
        close(fd);
        return;
    }
    if(strlen(path)+1+DIRSIZ+1>sizeof(buf)){
        printf("find: path too long\n");
        close(fd);
        return;
    }
    strcpy(buf, path);
    p=buf+strlen(buf);
    *p++='/';
    while(read(fd,&de,sizeof(de))==sizeof(de)){
        if(de.inum==0||!strcmp(de.name,".")||!strcmp(de.name,".."))
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ]=0;
        if(stat(buf, &st)<0){
            printf("find: cannot stat\n");
            continue;
        }
        if(st.type==T_DIR){
            find(buf,file);
        }
        else if(st.type==T_FILE&&!strcmp(de.name,file)){
            printf("%s\n",buf);
        }
    }
}

int main(int argc, char *argv[]){
    if(argc!=3){
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}