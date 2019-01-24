#ifndef _COMM_H
#define _COMM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define DATABASE "./system.db"   //数据库
#define LOG_PATH "./log.txt"     //日志文件

//消息的类型
#define USER_REGISTER  	 10   //注册消息
#define USER_LOGIN    	 20   //登录消息
#define USER_WORD      	 30   //查询单词消息
#define USER_SUCCESS     40   //成功消息
#define USER_FAILURE     50   //失败消息
#define USER_HADREG      60   //已经注册

typedef struct 
{
	char _username[25];
	char _password[25];
} __attribute__ ((__packed__))user_t;


typedef struct
{
	int type;
	int size;//记录单词解释大小
	union 
	{
		user_t uinfo;
		char  _word[100];
	}content;
//客户端填单词，服务器端填单词解释
#define word 		    content._word 
#define username	  content.uinfo._username
#define password  	content.uinfo._password
}__attribute__ ((__packed__))mhead_t;


#include "sql.h"



#endif
