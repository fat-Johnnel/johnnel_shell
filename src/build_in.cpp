#include "build_in.h"
using namespace std;
void echo_command(string args){
    //不要修改args
    auto it=args.begin();
    bool in_middle_of_string=false;
    bool single_space=false;
    char border_char='\0';
    while(it!=args.end()){
        if(*it=='\''){
            if(in_middle_of_string==false){
                in_middle_of_string=true;
                border_char='\'';
            }
            else if(border_char=='\''){
                in_middle_of_string=false;
                border_char='\0';
            }
            else{
                cout<<*it;
            }
        }
        else if(*it=='\"'){
            if(in_middle_of_string==false){
                in_middle_of_string=true;
                border_char='\"';
            }
            else if(border_char=='\"'){
                in_middle_of_string=false;
                border_char='\0';
            }
            else{
                cout<<*it;
            }
        }
        else if(*it==' ' || *it=='\n'){
            if(in_middle_of_string){
                cout<<*it;
            }
            else if(!single_space){
                cout<<*it;
                single_space=true;
            }
        }
        else{
            cout<<*it;
            single_space=false;
        }
        it++;
    }
}

pid_t Fork(void){
    pid_t pid;
    if((pid=fork())<0){
        cerr<<"fork error"<<endl;
        exit(1);
    }
    return pid;
}

sighandler_t Signal(int signum, sighandler_t handler){
    if(signal(signum,handler)==SIG_ERR){
        cerr<<"无法设置处理函数"<<endl;
        exit(1);
    }
    return handler;
}

int Tcsetpgrp(int fd,pid_t pgrp){
    int rc;
    if((rc=tcsetpgrp(fd,pgrp))<0){
        cerr<<"终端控制权转移失败"<<endl;
        exit(1);
    }
    return rc;
}

int Pipe(int pipefd[2]){
    int rc;
    if((rc=pipe(pipefd))<0){
        cerr<<"管道创建失败"<<endl;
        exit(1);
    }
    return rc;
}
string gettime(){
    time_t now=time(nullptr);
    struct tm * t=localtime(&now);
    char buffer[BUFFER_SIZE];
    strftime(buffer,BUFFER_SIZE,"%m-%d %H:%M",t);
    return string(buffer);
}

int export_cmd(string arg,char * command_path){
    regex path_pattern(R"(^COMMAND_PATH=\$COMMAND_PATH:(.*)$)");
    smatch match;
    if(regex_match(arg,match,path_pattern)){
        string new_path=match[1];
        if(command_path[strlen(command_path)-1]==':'){
            strcat(command_path,new_path.c_str());
        }
        else{
            strcat(command_path,":");
            strcat(command_path,new_path.c_str());
        }
        return 0;
    }
    else{
        cerr<<"export格式错误"<<endl;
        return 1;
    }
}