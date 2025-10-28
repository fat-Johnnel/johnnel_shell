#include "build_in.h"


using namespace std;
int main(int argc, char**argv){
    char input[BUFFER_SIZE];
    char filenames[20][BUFFER_SIZE];//最多读取20个文件
    int file_count=0;
    bool number=false;

    //解析参数
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            bool looped=false;
            for(int j=1;argv[i][j]!='\0';j++){
                looped=true;
                if(argv[i][j]=='n'){
                    number=true;
                }
                else{
                    cerr<<"cat:invalid option -\'"<<argv[i][j]<<"'"<<endl;
                    return 1;
                }
            }
            if(looped==false){
                strcpy(filenames[file_count],argv[i]);
                file_count++;
            }
        }
        else{
            strcpy(filenames[file_count],argv[i]);
            file_count++;
        }
    }
    // cout<<file_count<<endl;
    // cout << number << show_ends <<endl;
    // cout << filenames[0] <<endl;
    if(file_count==0){
        int line=1;
        while(fgets(input,BUFFER_SIZE,stdin)!=NULL){
            if(number){
                printf("%4d\t",line);
                line++;
            }
            printf("%s",input);
        }
        exit(0);
    }

    for(int i=0;i<file_count;i++){
        if(strcmp("-",filenames[i])==0){
            int line=1;
            while(fgets(input,BUFFER_SIZE,stdin)!=NULL){
                if(number){
                    printf("%4d\t",line);
                    line++;
                }
                printf("%s",input);
            }
            exit(0);
        }
        FILE *fp=fopen(filenames[i],"r");
        if(fp==NULL){
            cerr<<"cat: cannot open file "<<filenames[i]<<endl;
            continue;
        }
        int line=1;
        while(fgets(input,BUFFER_SIZE,fp)!=NULL){
            if(number){
                printf("%4d\t",line);
                line++;
            }
            printf("%s",input);

        }
    }
    return 0;

}