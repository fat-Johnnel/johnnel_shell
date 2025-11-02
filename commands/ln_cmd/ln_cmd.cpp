#include "build_in.h"
#include "fileOp.h"
using namespace std;

int main(int argc,char ** argv){
    struct arg_parse parse_result;
    if(arg_parser(&parse_result,argc,argv)<0){
        return -1;
    }
    int sysm_link=parse_result.options[letter_mapping('s')] || parse_result.options[letter_mapping('S')];
    string filename,linkname;
    if(parse_result.args.size()<2){
        cerr<<"ln:参数错误"<<endl;
        return -1;
    }
    filename=parse_result.args[0];
    linkname=parse_result.args[1];

    int res;
    if(sysm_link){
        res=symlink(filename.c_str(),linkname.c_str());
        if(res<0){
            perror("创建符号链接失败");
            return -1;
        }
    }
    else{
        res=link(filename.c_str(),linkname.c_str());
        if(res<0){
            perror("创建硬链接失败");
            return -1;
        }
    }
    return 0;
}