#ifndef _DATA_H
#define _DATA_H

#include "comm.h"

//得到日志文件的文件描述符
FILE *getFile();

//得到行号
int get_line();

//日志处理
void do_log(char *exp_word);



#endif
