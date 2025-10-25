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

        //管道切分
        vector<stringstream> pcommands;
        int command_count=1;
        string reader_str(reader);
        string::iterator pre=reader_str.begin();
        string::iterator it=reader_str.begin();
        while(it!=reader_str.end()){
            if(*it=='|'){
                string one_command(pre,it);
                pcommands.push_back(stringstream(one_command));
                command_count++;
                pre=it+1;
            }
            it++;
        }
        string one_command(pre,it);
        pcommands.push_back(stringstream(one_command));
        pid_t * pids=(pid_t*)malloc(sizeof(pid_t)*command_count);


        pid_t first_pid=-1;
        pid_t pid=-1;
        int (*pipefd)[2]=(int (*)[2])malloc(sizeof(int[2])*(command_count-1));
        int status;
        int pre_fd=STDIN_FILENO;
        for(int i=1;i<=command_count;i++){

            

            if(pid!=0){
                if(i!=command_count){
                    Pipe(pipefd[i-1]);
                }
                pid=Fork();
                
            }
            if(pid==0){
                Signal(SIGINT,SIG_DFL);
                stringstream &ss=pcommands[i-1];
                string command;
                ss >> command;
                if(i!=1){
                    dup2(pipefd[i-2][0],STDIN_FILENO);
                    close(pipefd[i-2][0]);
                    close(pipefd[i-2][1]);
                }
                if(i!=command_count){
                    dup2(pipefd[i-1][1],STDOUT_FILENO);
                    close(pipefd[i-1][0]);
                    close(pipefd[i-1][1]);
                }
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

                pids[i-1]=pid;
                setpgid(pid,pids[0]);

                if(i==1)
                    Tcsetpgrp(STDIN_FILENO,pid);
            }
        }
        for(int i=0;i<command_count;i++){
            waitpid(pids[i],&status,0);
        }
        Tcsetpgrp(STDIN_FILENO,getpgid(0));
    }
    return 0;
}
