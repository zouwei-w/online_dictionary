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

#define DATABASE "./system.db"   //���ݿ�
#define LOG_PATH "./log.txt"     //��־�ļ�

//��Ϣ������
#define USER_REGISTER  	 10   //ע����Ϣ
#define USER_LOGIN    	 20   //��¼��Ϣ
#define USER_WORD      	 30   //��ѯ������Ϣ
#define USER_SUCCESS     40   //�ɹ���Ϣ
#define USER_FAILURE     50   //ʧ����Ϣ
#define USER_HADREG      60   //�Ѿ�ע��

typedef struct 
{
	char _username[25];
	char _password[25];
} __attribute__ ((__packed__))user_t;


typedef struct
{
	int type;
	int size;//��¼���ʽ��ʹ�С
	union 
	{
		user_t uinfo;
		char  _word[100];
	}content;
//�ͻ�����ʣ�����������ʽ���
#define word 		content._word 
#define username	content.uinfo._username
#define password  	content.uinfo._password
}__attribute__ ((__packed__))mhead_t;






#include "data.h"





#endif
