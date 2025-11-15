#ifndef BUILD_IN_H
#define BUILD_IN_H
#define COMMAND_PATH "/home/Johnu/codes/johnnel_shell/bin/commands"
/*
echo
exit
cd 
pwd
*/
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <fcntl.h>
#include <utime.h>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>
#define BUFFER_SIZE 1024

struct arg_parse{
    int options[48];
    std::vector<std::string> args;
};


void echo_command(std::string args);
pid_t Fork(void);
sighandler_t Signal(int signum, sighandler_t handler);
int Tcsetpgrp(int fd,pid_t pgrp);
int Pipe(int pipefd[2]);
std::string gettime();
int export_cmd(std::string arg,char *command_path);
int Stat(const char *pathname, struct stat *statbuf);
int arg_parser(struct arg_parse * result,int argc,char **argv);
int letter_mapping(char c);
char demapping_letter(int index);
void wavy_to_home(char * path);
void wavy_to_home(std::string & path);
void home_to_wavy(char * path);
void home_to_wavy(std::string & path);
#endif
