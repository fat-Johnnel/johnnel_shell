/*
目前只支持两种参数：-a -l
*/

#include "build_in.h"

using namespace std;
int main(int argc, char**argv){
    char *dest_path=(char *)calloc(BUFFER_SIZE,sizeof(char));
    bool show_all=false;
    bool show_long=false;
    DIR * dirp;
    struct dirent * dp;
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            for(int j=1;j<strlen(argv[i]);j++){
                if(argv[i][j]=='a'){
                    show_all=true;
                }
                else if(argv[i][j]=='l'){
                    show_long=true;
                }
                else{
                    cerr<<"not support option: "<<argv[i][j]<<endl;
                    exit(1);
                }
            }
        }
        else{
            if(dest_path[0]=='\0')
                strcpy(dest_path,argv[i]);
            else{
                cerr<<"参数错误"<<endl;
                exit(1);
            }
        }
    }
    if(dest_path[0]=='\0'){
        getcwd(dest_path,BUFFER_SIZE);
    }
    dirp=opendir(dest_path);
    if(dirp==NULL){
        cerr<<"cannot open directory: "<<dest_path<<endl;
        exit(1);
    }
    while((dp=readdir(dirp))!=NULL){
        if(!show_all && dp->d_name[0]=='.'){
            continue;
        }
        if(show_long){
            struct stat st;
            struct passwd * pw;
            struct group  * gr;
            string full_path=string(dest_path)+"/"+string(dp->d_name);
            if(stat(full_path.c_str(),&st)<0){
                cerr<<"cannot access file: "<<full_path<<endl;
                continue;
            }
            cout<<((S_ISDIR(st.st_mode))?'d':'-');
            cout<<((st.st_mode & S_IRUSR)?'r':'-');
            cout<<((st.st_mode & S_IWUSR)?'w':'-');
            cout<<((st.st_mode & S_IXUSR)?'x':'-');
            cout<<((st.st_mode & S_IRGRP)?'r':'-');
            cout<<((st.st_mode & S_IWGRP)?'w':'-');
            cout<<((st.st_mode & S_IXGRP)?'x':'-');
            cout<<((st.st_mode & S_IROTH)?'r':'-');
            cout<<((st.st_mode & S_IWOTH)?'w':'-');
            cout<<((st.st_mode & S_IXOTH)?'x':'-');
            cout<<"\t"<<st.st_nlink;
            pw=getpwuid(st.st_uid);
            gr=getgrgid(st.st_gid);
            cout<<"\t"<<(pw!=NULL?pw->pw_name:to_string(st.st_uid));
            cout<<"\t"<<(gr!=NULL?gr->gr_name:to_string(st.st_gid));
            cout<<"\t"<<st.st_size;
            cout<<"\t"<<dp->d_name<<endl;
        }
        else{
            struct stat st;
            string full_path=string(dest_path)+"/"+string(dp->d_name);
            if(stat(full_path.c_str(),&st)<0){
                cerr<<"cannot access file: "<<full_path<<endl;
                continue;
            }
            if(S_ISDIR(st.st_mode)){
                cout<<"\033[34m"<<dp->d_name<<"\033[39m/   ";
            }
            else{
                cout<<dp->d_name<<"   ";
            }
        }
    }
    cout <<endl;
    closedir(dirp);
    free(dest_path);
    return 0;
}