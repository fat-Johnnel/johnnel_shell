#include "build_in.h"

#define  DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

using namespace std;
int main(int argc, char** argv) {
    for(int i=1;i<argc;i++){
        if(mkdir(argv[i],DIR_MODE)==-1){
            perror("mkdir error");
            return -1;
        }
    }
    return 0;
}