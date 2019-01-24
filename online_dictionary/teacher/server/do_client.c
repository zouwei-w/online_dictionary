#include "head.h"

int do_register(int sockfd,sqlite3 *pdb,char *_username,char *_password)
{
	char *errmsg;
	char buf[1024];
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	mhead_t *head = (mhead_t *)buf;	

	sprintf(sql,"select * from user_table where NAME='%s';",_username);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的用户名
	if(nrow == 0)
	{
		//录入数据库
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into user_table values('%s','%s');",_username,_password);
		EXEC_SQL(pdb,sql,errmsg);
		
		printf("ok ........\n");

		head->type = USER_SUCCESS;
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

	//注册失败,用户名存在
	}else{
		head->type = USER_FAILURE;
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

		printf("?????\n");
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_login(int sockfd,sqlite3 *pdb,char *_username,char *_password)
{
	char *errmsg;
	char buf[1024];
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	mhead_t *head = (mhead_t *)buf;	

	sprintf(sql,"select * from user_table where NAME='%s' and PASSWORD='%s';",_username,_password);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的用户名
	if(nrow == 0)
	{	
		head->type = USER_FAILURE;
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}
	//登录成功
	}else{
		head->type = USER_SUCCESS;
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_word(int sockfd,sqlite3 *pdb,char * pword)
{
	char *errmsg;
	char buf[1024];
	char **dbresult;
	int nrow,ncolumn;
	char sql[1024] = {0};
	mhead_t *head = (mhead_t *)buf;

	sprintf(sql,"select * from word_table where WORD='%s';",pword);
	if(sqlite3_get_table(pdb,sql,&dbresult,&nrow,&ncolumn,&errmsg) != 0)
	{
		fprintf(stderr,"sqlite3 get table error : %s.\n",errmsg);
		exit(EXIT_FAILURE);
	}
	
	//没有这样的单词
	if(nrow == 0)
	{	
		head->type = USER_FAILURE;
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}

	//单词存在
	}else{
	
		//注意:我的数据库文件在录入单词的时候，没有将'\r'字符去掉
		//告诉客户端单词存在
		head->type = USER_SUCCESS;
		strcpy(head->word,dbresult[ncolumn]);
		head->size = strlen(dbresult[ncolumn + 1]) - 1;//不包含'\r'字符
		if(send(sockfd,buf,sizeof(mhead_t),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}


		//发送单词解释的内容
		strcpy(buf,dbresult[ncolumn + 1]);
		buf[strlen(buf) - 1] = '\0';//这里去掉'\r'

		if(send(sockfd,buf,strlen(buf),0) < 0)
		{
			perror("Fail to send");
			exit(EXIT_FAILURE);
		}
	}

	sqlite3_free_table(dbresult);
	
	return 0;
}

int do_client(int sockfd,sqlite3 *pdb)
{
	int n;
	int count = 0;
	char buf[1024];
	mhead_t *head = (mhead_t *)buf;	

	//添加探测客户端是否存活机制		
	int keepAlive = 1; // 开启keepalive属性. 缺省值: 0(关闭)
	int keepIdle = 5; // 如果在5秒内没有任何数据交互,则进行探测. 缺省值:7200(s)
	int keepInterval = 10; // 探测时发探测包的时间间隔为10秒. 缺省值:75(s)
	int keepCount = 3; // 探测重试的次数. 全部超时则认定连接失效..缺省值:9(次)

	//打开KEEPALIVE选项
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));

	//设置探测时间
	setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount));  

	while(1)
	{
		count = 0;
		//接收协议头
		while(1)
		{
			n = recv(sockfd,buf + count,sizeof(mhead_t) - count,0);
			if(n <= 0){
				exit(EXIT_FAILURE);
			}

			count += n;
			if(count == sizeof(mhead_t))
				break;
		}

		switch(head->type)
		{
		case USER_REGISTER:
			do_register(sockfd,pdb,head->username,head->password);	
			break;
		
		case USER_LOGIN:
			do_login(sockfd,pdb,head->username,head->password);
			break;

		case USER_WORD:
			do_word(sockfd,pdb,head->word);
			break;

		defalut:
			exit(EXIT_SUCCESS);
		}	
	}

	return 0;
}
