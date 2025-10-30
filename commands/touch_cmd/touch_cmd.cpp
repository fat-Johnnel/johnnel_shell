#include "build_in.h"
#include <fcntl.h>
#include <utime.h>
#define FILE_MODE 0644

using namespace std;

int main(int argc,char **argv){
    for(int i=1;i<argc;i++){
        if(utime(argv[i],NULL)<0){
            int fd=open(argv[i],O_CREAT,FILE_MODE);
            if(fd<0){
                perror("touch error");
                exit(1);
            }
            else{
                close(fd);
            }
        }
    }
    return 0;
}