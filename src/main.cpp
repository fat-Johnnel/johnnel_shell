#include "build_in.h"
#define BUFFER_SIZE 1024

pid_t shell_pid;

using namespace std;
int main(int argc, char**argv){
    //>>>>>>>>>>>>>>>>>>>>>>>>>initilize<<<<<<<<<<<<<<<<<<<<<<
    char *reader=(char*)malloc(sizeof(char)*BUFFER_SIZE);
    char current_path[BUFFER_SIZE];
    char command_path[BUFFER_SIZE];
    strcpy(command_path,COMMAND_PATH);
    setenv("COMMAND_PATH",command_path,1);
    vector<string> path_list;
    path_list.push_back(string(command_path));
    shell_pid=getpid();

    //为了实现对信号的处理，这里将shell的进程单独成一个组
    setpgid(0,0); 
    //设置信号处理(ctrl+c 和 ctrl+d)
    Signal(SIGINT,SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);
    while(true){
        getcwd(current_path,BUFFER_SIZE);
        printf("%s $ ",current_path);
        if(fgets(reader,BUFFER_SIZE,stdin)==NULL)
        {
            break;
        }
        stringstream ss(reader);
        string command;
        ss >> command;


        //>>>>>>>>>>>>>>>>>>>build in command<<<<<<<<<<<<<<<<<<
        if(command=="echo"){
            string arg=ss.str().substr(5);
            echo_command(arg);
            continue;
        }

        else if(command=="exit"){
            break;
        }

        else if(command=="pwd"){
            char pwd_buffer[BUFFER_SIZE];
            getcwd(pwd_buffer,BUFFER_SIZE);
            cout<<pwd_buffer<<endl;
            continue;
        }

        else if(command=="cd"){
            string path;
            ss >> path;
            if(path.empty()){
                if(chdir(getenv("HOME"))!=0){
                    cerr<<"cannot open home directoty"<<endl;
                }
            }
            else{
                if(chdir(path.c_str())!=0){
                    cerr<<"no such file or directory: "<<path<<endl;
                }
            }
            continue;
        }
        //>>>>>>>>>>>>>>>>>>>>>>external command<<<<<<<<<<<<<<<<<<
        pid_t pid=Fork();
        int status;
        if(pid==0){
            setpgid(0,0);
            Signal(SIGINT,SIG_DFL);
            bool command_found=false;
            
            vector<char*> args;
            char * pushed=(char*)malloc(sizeof(char)*BUFFER_SIZE);
            strcpy(pushed,command.c_str());
            args.push_back(pushed);
            string arg;
            while(ss >> arg){
                pushed=(char*)malloc(sizeof(char)*BUFFER_SIZE);
                strcpy(pushed,arg.c_str());
                args.push_back(pushed);
            }
            args.push_back(nullptr);

            for(auto it=path_list.begin();it!=path_list.end();it++){
                string full_path=*it+"/"+command;
                if(execv(full_path.c_str(),args.data())>=0){
                    command_found=true;
                    break;
                }

            }

            for(auto arg_it=args.begin();arg_it!=args.end();arg_it++){
                if(*arg_it!=nullptr)
                    free(*arg_it);
            }

            if(!command_found){
                cerr<<"command not found: "<<command<<endl;
            }
            exit(0);
        }
        else{
            Tcsetpgrp(STDIN_FILENO,pid);
            waitpid(pid,&status,0);
            Tcsetpgrp(STDIN_FILENO,getpgid(0));
        }
    }
    return 0;
}
