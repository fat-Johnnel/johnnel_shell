#include "build_in.h"

#define BUFFER_SIZE 1024

using namespace std;
int main(int argc, char**argv){
    //>>>>>>>>>>>>>>>>>>>>>>>>>initilize<<<<<<<<<<<<<<<<<<<<<<
    char *reader=(char*)malloc(sizeof(char)*BUFFER_SIZE);
    char * current_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);


    while(true){
        getcwd(current_path,BUFFER_SIZE);
        printf("%s $",current_path);
        fgets(reader,BUFFER_SIZE,stdin);
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
        }
    }
    return 0;
}
