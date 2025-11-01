#ifndef FILE_OP_H
#define FILE_OP_H
#include "build_in.h"

void copy_file(const char * src,const char *dest);
void copy_recursive(const char * src,const char * dest);
int rm_recursive(const char * path);
#endif