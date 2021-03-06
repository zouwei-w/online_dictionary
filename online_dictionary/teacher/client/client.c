#include "head.h"

void help_info1()
{
	printf("\t------------------------------------------------\n\
			\r\t|               CYG   在线辞典                 |\n\
			\r\t|版本:0.0.1                                    |\n\
			\r\t|作者:草根老师                                 |\n\
			\r\t|功能:                                         |\n\
			\r\t|    [1] 登录                                  |\n\
			\r\t|    [2] 注册                                  |\n\
			\r\t|    [3] 退出                                  |\n\
			\r\t|注意:用户只有登录成功后才能进入查单词界面     |\n\
			\r\t------------------------------------------------\n");
	return;
}

void help_info2()
{
	printf("\t------------------------------------------------\n\
			\r\t|   欢迎进入单词查询系统，很高兴为您服务       |\n\
			\r\t|版本:0.0.1                                    |\n\
			\r\t|作者:草根老师                                 |\n\
			\r\t|功能:                                         |\n\
			\r\t|    [1] 查单词                                |\n\
			\r\t|    [2] 查询历史记录                          |\n\
			\r\t|    [3] 退出查询系统                          |\n\
			\r\t------------------------------------------------\n");
	return;
}

enum{
	LOGIN    = 1,
	REGISTER = 2,
	QUIT     = 3,
	QUERY    = 1,
	HISTORY  = 2,
};

int init_tcp(char *ip,char *port)
{
	int sockfd;
	struct sockaddr_in server_addr;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Fail to socket");
		exit(EXIT_FAILURE);
	}

	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(atoi(port));
	server_addr.sin_addr.s_addr = inet_addr(ip);

	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
	{
		perror("Fail to bind");
		exit(EXIT_FAILURE);
	}
	
	return sockfd;
}

int do_register(int sockfd)
{
	int n;
	int count = 0;
	char buf[1024];
	mhead_t *head = (mhead_t *)buf;	
	
	printf("\n您正在注册，请输入用户名和密码\n");

	head->type = USER_REGISTER;
	head->size = sizeof(mhead_t);

	printf("Input username : ");
	fgets(head->username,sizeof(head->username),stdin);
	head->username[strlen(head->username) - 1] = '\0';

	printf("Input password : ");
	fgets(head->password,sizeof(head->password),stdin);
	head->password[strlen(head->password) - 1] = '\0';
	
	//发给服务器端
	if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
	{
		perror("Fail to send");
		exit(EXIT_FAILURE);
	}


	//接收协议头
	bzero(&buf,sizeof(buf));
	while(1)
	{
		n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);
		if(n <= 0){
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

		count += n;
		if(count == sizeof(mhead_t))
			break;
	}
	
	if(head->type == USER_SUCCESS){
		printf("\n恭喜您，注册成功!\n");
		return 0;
	}else{
		printf("\n很遗憾，这个用户名已经被其它用户注册过了，请重新注册!\n");
		return -1;
	}
}

int do_login(int sockfd)
{
	int n;
	int count = 0;
	char buf[1024];
	mhead_t *head = (mhead_t *)buf;	

	printf("\n您正在登录，请输入用户名和密码\n");
	
	head->type = USER_LOGIN;
	head->size = sizeof(mhead_t);

	printf("Input username : ");
	fgets(head->username,sizeof(head->username),stdin);
	head->username[strlen(head->username) - 1] = '\0';

	printf("Input password : ");
	fgets(head->password,sizeof(head->password),stdin);
	head->password[strlen(head->password) - 1] = '\0';
	
	//发给服务器端
	if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
	{
		perror("Fail to send");
		exit(EXIT_FAILURE);
	}


	//接收协议头
	bzero(&buf,sizeof(buf));
	while(1)
	{
		n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);
		if(n <= 0){
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

		count += n;
		if(count == sizeof(mhead_t))
			break;
	}
	
	if(head->type == USER_SUCCESS){
		printf("\n恭喜您，登录成功!\n");
		return 0;
	}else{
		printf("\n登录失败，用户名或密码错误!\n");
		return -1;
	}
}

int write_time(FILE *fp,char *pword)
{
	time_t tim;
	struct tm *ptm;	

	tim = time(NULL);
	ptm = localtime(&tim);
	
	//定位到尾部开始写
	fseek(fp,0,SEEK_END);

	//写入时间
	fprintf(fp,"%d年%d月%d日   %d:%d\n",ptm->tm_year + 1900,ptm->tm_mon + 1,\
			ptm->tm_mday,ptm->tm_hour,ptm->tm_min);
	
	//写入单词 
	fprintf(fp,"%s        ",pword);

	return 0;
}

int recv_word_explain(int sockfd,FILE *fp)
{
	int  n;
	int  size;
	char buf[1024];
	int count = 0;
	mhead_t *head = (mhead_t *)buf;

	//接收协议头
	while(1)
	{
		n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);
		if(n <= 0){
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

		count += n;
		if(count == sizeof(mhead_t))
			break;
	}
	
	if(head->type == USER_SUCCESS){
		
		//获取单词解释的长度
		size = head->size;
		count = 0;
	
		printf("\n");

		//向文件中写入当前时间和单词
		write_time(fp,head->word);

		while(1)
		{
			//接收单词解释
			//单词将解释可能很大，这里循环接收，每接收一部分
			//就输出一部分，直到接收到所有的单词解释
			n = recv(sockfd,buf,sizeof(buf) - 1,0);
			if(n <= 0){
				perror("Fail to recv");
				exit(EXIT_FAILURE);
			}
			buf[n] = '\0';
			printf("%s",buf);
				
			//向文件中写入内容
			fprintf(fp,"%s",buf);

			count += n;

			if(count == size)
				break;
		}

		//向文件中写入换行符
		fprintf(fp,"\n\n");
		fflush(fp);

		printf("\n");
		return 0;
	}else{
		printf("No such word:%s!\n",head->word);
		return -1;
	}
}

int query_word(int sockfd,FILE *fp)
{
	int n;
	int count = 0;
	char buf[1024];
	mhead_t *head = (mhead_t *)buf;	

	head->type = USER_WORD;
	head->size = sizeof(mhead_t);
	
	printf("\n您，正在查询单词!\n");
	printf("Input word : ");
	fgets(head->word,sizeof(head->word),stdin);
	head->word[strlen(head->word) - 1] = '\0';

	if(strncmp(head->word,".quit",5) == 0)
		exit(EXIT_SUCCESS);

	//发给服务器端
	if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
	{
		perror("Fail to send");
		exit(EXIT_FAILURE);
	}

	//接收单词的解释
	recv_word_explain(sockfd,fp);

	return 0;
}

int show_history(FILE *fp)
{
	char buf[1024];

	fseek(fp,0,SEEK_SET);

	while(fgets(buf,sizeof(buf),fp) != NULL)
	{
		printf("%s",buf);
	}
	printf("\n");
	
	return 0;
}

int do_word(int sockfd)
{
	int cmd;
	FILE *fp;

	//打开.history文件，用来保存查询记录
	fp = fopen(".history","a+");
	if(fp == NULL){
		perror("Fail to fopen");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		help_info2();
		
		printf("\n请选择>");
		scanf("%d",&cmd);
		getchar();

		switch(cmd)
		{
			case QUERY:
				 query_word(sockfd,fp);
				 break;

			case HISTORY:
				 show_history(fp);
				 break;
			
			case QUIT:
				exit(EXIT_SUCCESS);

			default:
				printf("Unknow cmd.\n");
				continue;
		}
	}
	
	return 0;
}

int do_task(int sockfd)
{
	int cmd;

	while(1)
	{
		help_info1();
		
		printf("\n\n请选择>");
		scanf("%d",&cmd);
		getchar();

		switch(cmd)
		{
			case LOGIN:
				if(do_login(sockfd) < 0)  continue;
				goto next;
			
			case REGISTER:
				if(do_register(sockfd) < 0) continue;
				goto next;
			
			case QUIT:
				exit(EXIT_SUCCESS);

			default:
				printf("Unknow cmd.\n");
				continue;
		
		}
	}

next:
	do_word(sockfd);
	
	return 0;
}


//./server ip port db
//数据库中已经手动创建了2个表:user_tale,word_table
int main(int argc,char *argv[])
{
	int sockfd;
	int addr_len = sizeof(struct sockaddr);
	struct sockaddr_in peer_addr;

	if(argc < 3)
	{
		fprintf(stderr,"Usage : %s argv[1].\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	sockfd = init_tcp(argv[1],argv[2]);

	do_task(sockfd);
	
	exit(EXIT_SUCCESS);
}
