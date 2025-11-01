#include "build_in.h"
#include "fileOp.h"
using namespace std;


int main(int argc,char **argv){
    
    if(argc<3){
        cerr<<"cp: 缺少文件操作数"<<endl;
        return -1;
    }
    int recursive =0;
    char src[BUFFER_SIZE]={0};
    char dest[BUFFER_SIZE]={0};
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"-r")==0 || strcmp(argv[i],"-R")==0){
            recursive=1;
        }
        else if(src[0]=='\0'){
            strcpy(src,argv[i]);
        }
        else if(dest[0]=='\0'){
            strcpy(dest,argv[i]);
        }
        else{
            cerr<<"cp: 多余的操作数 "<<argv[i]<<endl;
            return -1;
        }
    }
    struct stat st;
    Stat(src,&st);
    if(!S_ISREG(st.st_mode) && recursive==0){
        cerr<<"cp: -r 未指定，无法复制目录： "<<src<<endl;
        return -1;
    }
    if(recursive){
        copy_recursive(src,dest);
    }
    else{
        copy_file(src,dest);
    }
    return 0;

}