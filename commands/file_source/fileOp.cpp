#include "fileOp.h"
using namespace std;

void copy_file(const char * src,const char * dest)
{
    int src_fd=open(src,O_RDONLY);
    if(src_fd<0){
        cerr<<"cannot open source file: "<<src<<endl;
        exit(1);
    }
    size_t dlen=strlen(dest);
    char dest_copy[BUFFER_SIZE];
    strcpy(dest_copy,dest);
    if(dest[dlen-1]=='/') dest_copy[dlen-1]='\0';
    struct stat st;
    if(stat(dest_copy,&st)!=0)
        if(S_ISDIR(st.st_mode)){
            strcat(dest_copy,"/");
            const char * base_name=strrchr(src,'/');
            if(base_name==NULL){
                strcat(dest_copy,src);
            }
            else{
                strcat(dest_copy,base_name+1);
            }
        }
    int dest_fd=open(dest_copy,O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(dest_fd<0){
        cerr<<"cannot open destination file: "<<dest_copy<<" skipping "<<endl;
        close(src_fd);
        return;
    }
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while((bytes_read=read(src_fd,buffer,BUFFER_SIZE))>0){
        ssize_t bytes_written=write(dest_fd,buffer,bytes_read);
        if(bytes_written<0 || bytes_written!=bytes_read){
            cerr<<"读写文件时发生错误："<<dest<<endl;
            close(src_fd);
            close(dest_fd);
            exit(1);
        }
    }
}

void copy_recursive(const char * src,const char * dest){
    struct stat st;
    Stat(src,&st);
    if(S_ISDIR(st.st_mode)){
        size_t dlen=strlen(dest);
        char dest_copy[BUFFER_SIZE];
        strcpy(dest_copy,dest);
        if(dest[dlen-1]=='/') {
            char src_copy[BUFFER_SIZE];
            strcpy((char*)src_copy,src);
            char * base_name=strrchr(src_copy,'/');
            if(base_name==NULL){
                strcat(dest_copy,src);
            }
            else{
                strcat(dest_copy,base_name+1);
            }
        }
        
        DIR * dir=opendir(src);
        if(dir==NULL){
            cerr<<"无法打开目录："<<src<<endl;
            exit(1);
        }
        if(mkdir(dest_copy,st.st_mode)==-1){
            perror("创建目录失败");
            closedir(dir);
            exit(1);
        }
        struct dirent * readent;
        while((readent=readdir(dir))!=NULL){
            if(strcmp(readent->d_name,".")==0 || strcmp(readent->d_name,"..")==0){
                continue;
            }
            string new_src=string(src)+"/"+string(readent->d_name);
            string new_dest=string(dest)+"/"+string(readent->d_name);
            copy_recursive(new_src.c_str(),new_dest.c_str());
        }
        closedir(dir);
    }
    else if(S_ISLNK(st.st_mode)){
        char link_target[BUFFER_SIZE];
        ssize_t len=readlink(src,link_target,BUFFER_SIZE-1);
        if(len<0){
            cerr<<"读取符号链接失败："<<src<<endl;
            exit(1);
        }
        link_target[len]='\0';
        if(symlink(link_target,dest)<0){
            cerr<<"创建符号链接失败："<<dest<<endl;
            exit(1);
        }

    }
    else{
        copy_file(src,dest);
    }
}

int rm_recursive(const char * path){
    struct stat st;
    Stat(path,&st);
    if(S_ISREG(st.st_mode))
    {
        if(remove(path)<0){
            perror("删除文件失败");
            return -1;
        }
    }
    else if(S_ISDIR(st.st_mode)){
        DIR *dir=opendir(path);
        if(dir==NULL){
            perror("无法打开目录");
            return -1;
        }
        struct dirent * dp;
        while((dp=readdir(dir))!=NULL){
            if(strcmp(dp->d_name,".")==0 || strcmp(dp->d_name,"..")==0){
                continue;
            }
            string child_path=string(path)+"/"+string(dp->d_name);
            if(rm_recursive(child_path.c_str())<0){
                closedir(dir);
                return -1;
            }
        }
        closedir(dir);
        if(rmdir(path)<0){
            perror("删除目录失败");
            return -1;
        }
    }
    return 0;
}
