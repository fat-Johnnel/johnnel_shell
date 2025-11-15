#include "build_in.h"
using namespace std;

void wavy_to_home(char * path){
    char *home_path=getenv("HOME");
    char buffer[BUFFER_SIZE];
    if(path[0]=='~'){
        strcpy(buffer,home_path);
        strcat(buffer,path+1);
        strcpy(path,buffer);
    }
}

void wavy_to_home(string & path){
    char *home_path=getenv("HOME");
    if(path[0]=='~'){
        path=string(home_path)+path.substr(1);
    }
}

void home_to_wavy(char * path){
    char buffer[BUFFER_SIZE];
    char *home_path=getenv("HOME");
    size_t home_len=strlen(home_path);
    if(strncmp(path,home_path,home_len)==0){
        buffer[0]='~';
        strcpy(buffer+1,path+home_len);
        strcpy(path,buffer);
    }
}

void home_to_wavy(std::string & path){
    char *home_path=getenv("HOME");
    size_t home_len=strlen(home_path);
    if(path.substr(0,home_len)==string(home_path)){
        path="~"+path.substr(home_len);
    }
}


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


int Stat(const char *pathname, struct stat *statbuf){
    int rc;
    if((rc=stat(pathname,statbuf))<0){
        cerr<<"获取文件状态失败："<<pathname<<endl;
        exit(1);
    }
    return rc;
}

int letter_mapping(char c){
    if(c>='a' && c<='z'){
        return c-'a';
    }
    else if(c>='A' && c<='Z'){
        return c-'A'+26;
    }
    else{
        return -1;
    }
}
char demapping_letter(int index){
    if(index>=0 && index<=25){
        return 'a'+index;
    }
    else if(index>=26 && index<=51){
        return 'A'+(index-26);
    }
    return 0;
}


int arg_parser(struct arg_parse * result,int argc,char ** argv){
    for(int i=0;i<48;i++) result->options[i]=0;
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            for(int j=1;j<strlen(argv[i]);j++){
                int index=letter_mapping(argv[i][j]);
                if(index==-1){
                    cerr<<"不支持的选项："<<argv[i][j]<<endl;
                    return -1;
                }
                result->options[index]=1;
            }
        }
        else{
            result->args.push_back(string(argv[i]));
        }
    }
    return 0;
}
