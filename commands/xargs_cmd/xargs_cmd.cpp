#include "build_in.h"

using namespace std;

//受busybox启发，使用结构化数据
class args{
public:
    char * command;
    char ** argv;
    int argc;
    bool set_args_max;
    int args_max;
    int char_count_init;
    long arg_max;
    args(int pargc,char ** pargv){
        bool command_found=false;
        command=(char*)malloc(sizeof(char)*BUFFER_SIZE);
        argv=(char **)calloc(BUFFER_SIZE,sizeof(char *));
        argc=0;
        args_max=-1;
        set_args_max=false;
        char_count_init=0;
        arg_max=sysconf(_SC_ARG_MAX);
        for(int i=1;i<pargc;i++){
            if(command_found==false){
                if(pargv[i][0]=='-'){
                    if(pargv[i][1]=='n'){
                        set_args_max=true;
                    }
                    else{
                        cerr<<"xargs:invalid option -\'"<<pargv[i][1]<<"'"<<endl;
                        exit(1);
                    }
                }
                else if(set_args_max==true && args_max==-1){
                    args_max=atoi(pargv[i]);
                    if(args_max<=0){
                        cerr<<"xargs:invalid max arg \'"<<pargv[i]<<"\'"<<endl;
                        exit(1);
                    }
                }
                else{
                    strcpy(command,pargv[i]);
                    command_found=true;
                    argc++;
                    argv[argc-1]=(char *)malloc(sizeof(char)*BUFFER_SIZE);
                    strcpy(argv[argc-1],pargv[i]);
                }
            }
            
            else{
                argc++;
                argv[argc-1]=(char *)malloc(sizeof(char)*BUFFER_SIZE);
                strcpy(argv[argc-1],pargv[i]);
                char_count_init+=strlen(pargv[i])+1;
            }
        }
        char_count_init+=strlen(command)+1;
    }
    ~args(){
        free(command);
        for(int i=0;i<argc;i++){
            free(argv[i]);
        }
        free(argv);
    }

    int get_arg(){
        int char_count=char_count_init;
        bool get_failed=true;
        char *input=(char *)malloc(sizeof(char)*BUFFER_SIZE);
        while(cin>>input){
            get_failed=false;
            if((set_args_max==true && argc<args_max && char_count+strlen(input)+1<arg_max) || (char_count+strlen(input)+1<arg_max)){
                argc++;
                argv[argc-1]=(char *)malloc(sizeof(char)*BUFFER_SIZE);
                strcpy(argv[argc-1],input);
                char_count+=strlen(input)+1;
            }
            else{
                break;
            }
        }
        free(input);
        if(get_failed==true){
            return -1;
        }
        return 0;
    }

    int exec_cmd(string cmd_path){
        argv[argc]=nullptr;
        pid_t pid=fork();
        if(pid<0){
            cerr<<"xargs:fork error"<<endl;
            exit(1);
        }
        else if(pid==0){
            cmd_path+="/"+string(command);
            cout<<"Executing command: "<<cmd_path<<endl;
            for(int i=0;i<argc;i++){
                cout<<"argv["<<i<<"]:"<<argv[i]<<endl;
            }
            strcpy(argv[0],cmd_path.c_str());
            if(execv(cmd_path.c_str(),argv)<0){
                cerr<<"xargs:exec error for command \'"<<command<<"\'"<<endl;
                perror("execv");
                exit(1);
            }
        }
        else{
            int status;
            waitpid(pid,&status,0);
        }
        return 0;
    }


};



int main(int argc,char ** argv){
    args xargs(argc,argv);
    char *path=(char *)malloc(sizeof(char)*BUFFER_SIZE);
    if(getenv("COMMAND_PATH")==NULL){
        cerr<<"xargs:environment variable COMMAND_PATH not set"<<endl;
        exit(1);
    }
    else{
        strcpy(path,getenv("COMMAND_PATH"));
    }
    while(xargs.get_arg()==0){
        string cmd_path=string(path);
        xargs.exec_cmd(cmd_path);
    }
    return 0;
}