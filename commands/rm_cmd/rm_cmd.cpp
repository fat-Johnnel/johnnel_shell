#include "build_in.h"
#include "fileOp.h"

using namespace std;

int main(int argc,char ** argv){
    if(argc<2){
        cerr<<"rm:参数错误"<<endl;
        return -1;
    }
    for(int i=1;i<argc;i++){
        rm_recursive(argv[i]);
    }
    return 0;
}