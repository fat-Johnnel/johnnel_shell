#include "build_in.h"

#define BUFFER_SIZE 1024

using namespace std;
int main(int argc, char**argv){
    char *reader=(char*)malloc(sizeof(char)*BUFFER_SIZE);
    char * current_path=(char*)malloc(sizeof(char)*BUFFER_SIZE);


    while(true){
        getcwd(current_path,BUFFER_SIZE);
        printf("%s $",current_path);
        fgets(reader,BUFFER_SIZE,stdin);
        stringstream ss(reader);
        string command;
        ss >> command;
        if(command=="echo"){
            string arg=ss.str().substr(5);
            echo_command(arg);
            continue;
        }
        
    }
    return 0;
}
