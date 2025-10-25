#include "build_in.h"
#define BUFFER_SIZE 1024

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
    while(true){
        getcwd(current_path,BUFFER_SIZE);
        printf("%s $",current_path);
        fgets(reader,BUFFER_SIZE,stdin);
        stringstream ss(reader);
        string command;
        ss >> command;


        //为了实现对信号的处理，这里将shell的进程单独成一个组
        setpgid(0,0); 

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
            bool command_found=false;
            for(auto it=path_list.begin();it!=path_list.end();it++){
                string full_path=*it+"/"+command;
                vector<char*> args_vector;
                args_vector.push_back(const_cast<char*>(command.c_str()));
                string arg;
                while(ss >> arg){
                    args_vector.push_back(const_cast<char*>(arg.c_str()));
                }
                args_vector.push_back(nullptr);
                if(execv(full_path.c_str(),args_vector.data())>=0){
                    command_found=true;
                    break;
                }
            }
            if(!command_found){
                cerr<<"command not found: "<<command<<endl;
            }
            exit(0);
        }
        else{
            waitpid(pid,&status,0);
        }
    }
    return 0;
}
