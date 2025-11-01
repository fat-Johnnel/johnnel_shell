#include "build_in.h"

pid_t shell_pid;

using namespace std;
int main(int argc, char**argv){
    //>>>>>>>>>>>>>>>>>>>>>>>>>initilize<<<<<<<<<<<<<<<<<<<<<<
    char *reader;
    char current_path[BUFFER_SIZE];
    char command_path[BUFFER_SIZE];
    strcpy(command_path,COMMAND_PATH);
    setenv("COMMAND_PATH",command_path,1);
    shell_pid=getpid();
    char username[BUFFER_SIZE];
    char hostname[BUFFER_SIZE];
    getlogin_r(username,BUFFER_SIZE);
    struct passwd * pw=getpwuid(getuid());
    if(pw!=NULL){
        strcpy(username,pw->pw_name);
    }
    else {
        strcpy(username,"unknown");
    }
    gethostname(hostname,BUFFER_SIZE);

    //为了实现对信号的处理，这里将shell的进程单独成一个组
    setpgid(0,0); 
    //设置信号处理(ctrl+c 和 ctrl+d)
    Signal(SIGINT,SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);
    while(true){
        strcpy(command_path,getenv("COMMAND_PATH"));
        int index=0;
        vector<string> path_list;
        for(int i=0;i<strlen(command_path);i++){
            if(command_path[i]==':'){
                path_list.push_back(string(command_path+index,command_path+i));
                index=i+1;
            }
        }
        path_list.push_back(string(command_path+index,command_path+strlen(command_path)));

        string current_time=gettime();
        getcwd(current_path,BUFFER_SIZE);
        printf("%s@%s %s %s ",username,hostname,current_path,current_time.c_str());
        if((reader=readline("$ "))==NULL)
        {
            break;
        }

        //管道切分
        vector<stringstream> pcommands;
        int command_count=1;
        string reader_str(reader);
        free(reader);
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

        //重定向检查
        bool dup_check=false;
        int dfd[2]={0};
        for(int i=0;i<command_count;i++){
            stringstream &ss=pcommands[i];
            stringstream new_ss;
            string signle_command=ss.str();
            size_t in_pos=signle_command.find("<");
            size_t out_pos=signle_command.find(">");
            if(in_pos!=string::npos && out_pos!=string::npos){
                cerr<<"非法指令：输入输出重定向只能有一个"<<endl;
                dup_check=true;
                break;
            }
            if(in_pos!=string::npos){
                if(i!=0){
                    cerr<<"非法指令：输入重定向只能出现在第一个命令"<<endl;
                    dup_check=true;
                    break;
                }
                else
                {
                    string before=signle_command.substr(0,in_pos);
                    string after=signle_command.substr(in_pos+1);
                    stringstream after_ss(after);
                    string filename;
                    after_ss >> filename;
                    dfd[0]=open(filename.c_str(),O_RDONLY);
                    if(dfd[0]<0){
                        cerr<<"cannot open file: "<<filename<<endl;
                    }
                    new_ss << before;
                    pcommands[i].str(new_ss.str());
                    pcommands[i].clear();
                    
                }
            }
            else if(out_pos!=string::npos){
                if(i!=command_count-1){
                    cerr<<"非法指令：输出重定向只能出现在最后一个命令"<<endl;
                    dup_check=true;
                    break;
                }
                else{
                    string before=signle_command.substr(0,out_pos);
                    string after=signle_command.substr(out_pos+1);
                    stringstream after_ss(after);
                    string filename;
                    after_ss >> filename;
                    dfd[1]=open(filename.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0644);
                    if(dfd[1]<0){
                        cerr<<"cannot open file: "<<filename<<endl;
                    }
                    new_ss << before;
                    pcommands[i].str(new_ss.str());
                    pcommands[i].clear();
                }
            }
            else{
                ;//没有重定向
            }
            
        }
        if(dup_check){
            continue;
        }

        //是否后台运行
        vector<bool> is_back_ground(command_count,false);
        for(int i=0;i<command_count;i++){
            stringstream &ss=pcommands[i];
            string command_line=ss.str();
            for(auto it:command_line){
                if(it=='&'){
                    is_back_ground[i]=true;
                    auto new_end=remove(command_line.begin(),command_line.end(),'&');
                    command_line.erase(new_end,command_line.end());
                    pcommands[i].str(command_line);
                    pcommands[i].clear();
                    break;
                }
            }
        }
        
        pid_t first_pid=-1;
        pid_t pid=-1;
        pid_t back_gpid=-1;
        int (*pipefd)[2]=(int (*)[2])malloc(sizeof(int[2])*(command_count-1));
        int status;
        int pre_fd=STDIN_FILENO;
        for(int i=1;i<=command_count;i++){
            stringstream &ss=pcommands[i-1];
            string command;
            ss >> command;
            if(pid!=0){
                if(i!=command_count){
                    Pipe(pipefd[i-1]);
                }

                //执行cd
                if(command=="cd"){
                    string path;
                    ss >> path;
                    if(path.empty() || path=="~"){
                        if(chdir(getenv("HOME"))!=0){
                            cerr<<"cannot open home directoty"<<endl;
                        }
                    }
                    else{
                        if(chdir(path.c_str())!=0){
                            cerr<<"no such file or directory: "<<path<<endl;
                        }
                    }
                    break;
                }
                else if(command=="export"){
                    string var;
                    ss>>var;
                    export_cmd(var,command_path);
                    setenv("COMMAND_PATH",command_path,1);
                    break;
                }
                else
                    pid=Fork();
                
            }
            if(pid==0){
                Signal(SIGINT,SIG_DFL);
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
                if(i==1 && dfd[0]!=0){
                    dup2(dfd[0],STDIN_FILENO);
                    close(dfd[0]);
                }
                if(i==command_count && dfd[1]!=0){
                    dup2(dfd[1],STDOUT_FILENO);
                    close(dfd[1]);
                }
                //>>>>>>>>>>>>>>>>>>>build in command<<<<<<<<<<<<<<<<<<
                if(command=="echo"){
                    string arg=ss.str().substr(5);
                    echo_command(arg);
                    exit(0);
                }

                else if(command=="exit"){
                    exit(-2);
                }

                else if(command=="pwd"){
                    char pwd_buffer[BUFFER_SIZE];
                    getcwd(pwd_buffer,BUFFER_SIZE);
                    cout<<pwd_buffer<<endl;
                    exit(0);
                }

                else if(command=="sleep"){
                    float sleep_time;
                    ss >> sleep_time;
                    usleep((useconds_t)(sleep_time*1000000));
                    exit(0);
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
                if(dfd[0]!=0)
                    close(dfd[0]);
                if(dfd[1]!=0)
                    close(dfd[1]);
                exit(0);
            }
            else{  
                if(!is_back_ground[i-1]){
                    if(first_pid==-1)
                    {
                        first_pid=pid;
                        setpgid(pid,first_pid);
                        Tcsetpgrp(STDIN_FILENO,pid);
                    }
                    else setpgid(pid,first_pid);
                }
                else{
                    if(back_gpid==-1){
                        back_gpid=pid;
                    }
                    setpgid(pid,back_gpid);
                }
                pids[i-1]=pid;
                
            }
        }
        for(int i=0;i<command_count-1;i++){
            close(pipefd[i][0]);
            close(pipefd[i][1]);
        }
        bool exit_flag=false;
        for(int i=0;i<command_count;i++){
            if(!is_back_ground[i])
                waitpid(pids[i],&status,0);
            if(WEXITSTATUS(status)==-2){
                exit_flag=true;
            }
        }
        Tcsetpgrp(STDIN_FILENO,getpgid(0));
        free(pids);
        free(pipefd);
        if(exit_flag){
            break;
        }
    }
    return 0;
}
