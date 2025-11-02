#include "build_in.h"
#include "fileOp.h"
using namespace std;

int main(int argc,char ** argv){
    if(argc<3){
        cerr<<"mv:参数错误"<<endl;
        return -1;
    }
    char src[BUFFER_SIZE]={0};
    char dest[BUFFER_SIZE]={0};
    for(int i=1;i<argc;i++){
        if(src[0]=='\0'){
            strcpy(src,argv[i]);
        }
        else if(dest[0]=='\0'){
            strcpy(dest,argv[i]);
        }
        else{
            cerr<<"mv: 多余的操作数 "<<argv[i]<<endl;
            return -1;
        }
    }
    struct stat st;
    Stat(src,&st);
    if(S_ISDIR(st.st_mode)){
        cerr<<"mv: 不能将目录移动到文件"<<endl;
        return -1;
    }
    copy_file(src,dest);
    if(remove(src)<0){
        perror("删除源文件失败");
        return -1;
    }
    return 0;
}