#include "comm.h"

#define MFULSH {char ch=0;while((ch=getchar()) != '\n' && ch != EOF);}

#define DEST_PORT 80
#define DEST_IP_ADDR "47.107.241.203"
#define DEST_IP_BY_NAME "www.weiosiew.top"

void do_register(int sockfd);
void do_login(int sockfd);
void do_catchwords(int sockfd);
void catch_history();
void catch_main(int sockfd);

void help_info1()
{
    printf("\n\t---------------------------------------------------\n\
        \r\t|          ZW    ONLINE DICTIONARY                |\n\
        \r\t|Version:0.0.1                                    |\n\
        \r\t|Author:W7vo                                      |\n\
        \r\t|Function:                                        |\n\
        \r\t|    [1] Register                                 |\n\
        \r\t|    [2] Login                                    |\n\
        \r\t|    [3] Quit                                     |\n\
        \r\t|NODE:Users need to login successfully to use it! |\n\
        \r\t---------------------------------------------------\n");
    return;
}

void help_info2()
{
    printf("\n\t---------------------------------------------------\n\
        \r\t|            Welcome! Glad to serve you!          |\n\
        \r\t|Version:0.0.1                                    |\n\
        \r\t|Author:W7vo                                      |\n\
        \r\t|Function:                                        |\n\
        \r\t|    [1] Words to explain                         |\n\
        \r\t|    [2] History                                  |\n\
        \r\t|    [3] Quit                                     |\n\
        \r\t---------------------------------------------------\n");
    return;
}


int Init_socket(void)
{
//    int ret;
    int sockfd;
    struct sockaddr_in client_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Failed to socket");
        return -1;
    }

    struct hostent* hostInfo = gethostbyname(DEST_IP_BY_NAME);
    if(NULL == hostInfo){
        printf("hostInfo is null\n");
        return -6;
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(DEST_PORT);

    printf("Ip address = %s \n",inet_ntoa(*((struct in_addr*)hostInfo->h_addr)));
    memcpy(&client_addr.sin_addr, &(*hostInfo->h_addr_list[0]), hostInfo->h_length);

    if (connect(sockfd, (struct sockaddr*)(&client_addr), sizeof(client_addr)) < 0)
    {
        perror("connect error\n");
        exit(1);
    }
    else
    {
        printf("connect successful\n");
    }
    
    return sockfd;
}

int send_message(int sockfd)
{
    int ret;
    int cmd;
    
    while(1)
    {
        help_info1();
        printf("cmd > ");
        ret = scanf("%d",&cmd);MFULSH;
        if(ret == 1)
        {
            switch(cmd)
            {
            case 1: //注册
                do_register(sockfd);
                break;
            case 2: //登录
                do_login(sockfd);
                break;
            case 3: //退出
                exit(EXIT_SUCCESS);
                break;
            default:printf("Input error!!!\n");
            }
        }else{
            printf("Input error!!!\n");
        }
    }
    return 0;
}



void *thread_send(void *p)
{
    //int ret;
    int sockfd = *(int *)p;
    while(1)
    {
        send_message(sockfd);
    }

    pthread_exit(NULL);
}



int main(int argc, const char *argv[])
{
    int ret;
    int sockfd;
    pthread_t thread1;

    sockfd = Init_socket(); //初始化套接字
    ret = pthread_create(&thread1, NULL, thread_send, &sockfd);//开启线程发送数
    if(ret < 0)
    {
        perror("pthread create err:");
        return -1;
    }

    pthread_join(thread1,NULL);

    return 0;
}


void do_register(int sockfd)
{
    mhead_t head;
    mhead_t re_head;

    printf("\nRegistering! Please Input Username and Password!\n");

    head.type = USER_REGISTER;
    head.size = sizeof(mhead_t);

    printf("Input username : ");
    fgets(head.username,sizeof(head.username),stdin);
    head.username[strlen(head.username) - 1] = '\0';

    printf("Input password : ");
    fgets(head.password,sizeof(head.password),stdin);
    head.password[strlen(head.password) - 1] = '\0';

    send(sockfd, &head, sizeof(mhead_t), 0);

    recv(sockfd, &re_head, sizeof(mhead_t), 0);
    switch(re_head.type)
    {
    case USER_SUCCESS:
        printf("REGISTER SUCCESS!\n");
        catch_main(sockfd); //注册成功就跳转到查询界面
        break;

    case USER_FAILURE:
        printf("REGISTER FAILED!\n");
        break;

    case USER_HADREG:
        printf("HAD REGISTER!\n");
        break;
    }

}

void do_login(int sockfd)
{
    mhead_t head;
    mhead_t re_head;

    printf("\nSigning on! Please Input Username and Password!\n");

    head.type = USER_LOGIN;
    head.size = sizeof(mhead_t);

    printf("Input username : ");
    fgets(head.username,sizeof(head.username),stdin);
    head.username[strlen(head.username) - 1] = '\0';

    printf("Input password : ");
    fgets(head.password,sizeof(head.password),stdin);
    head.password[strlen(head.password) - 1] = '\0';

    send(sockfd, &head, sizeof(mhead_t), 0);

    recv(sockfd, &re_head, sizeof(mhead_t), 0);
    switch(re_head.type)
    {
    case USER_SUCCESS:
        printf("LOGIN SUCCESS!\n");
        catch_main(sockfd); //登录成功就跳转到查询界面
        break;

    case USER_FAILURE:
        printf("LOGIN FAILED!\n");
        break;
    }
}

void do_catchwords(int sockfd)
{
    int n;
    int size;
    int count;
    char buf[1024];
    char dest[4096];
    mhead_t head;
    mhead_t re_head;

    printf("word > ");
    fgets(head.word, sizeof(head.word) - 1, stdin);
    head.word[strlen(head.word)-1] = '\0';

    head.type = USER_WORD;
    send(sockfd, &head, sizeof(mhead_t), 0);

    n = recv(sockfd, &re_head, sizeof(mhead_t), 0);
    if(n <= 0)
    {
        perror("Failed to recv");
        exit(EXIT_FAILURE);
    }

    count = 0;
    size = re_head.size;  //单词解释的大小

    if(re_head.type == USER_SUCCESS)
    { 
        sprintf(dest,"%s|", head.word);//查询的单词
        while(1)
        {
            n = recv(sockfd, buf, sizeof(buf) - 1, 0);
            if(n <= 0){
                perror("Fail to recv");
                exit(EXIT_FAILURE);
            }
            buf[n] = '\0';
            strcat(dest,buf);
            count += n;

            if(count == size)
                break;
        }
        strcat(dest,"\0\n");
        printf("%s\n\n\n",dest);
        do_log(dest);
    }else{
        printf("%s\n",re_head.word);
    }


}

void catch_main(int sockfd)
{
    int ret;
    int cmd;
    while(1)
    {
        help_info2();
        printf("cmd > ");
        ret = scanf("%d",&cmd);MFULSH;
        if(ret == 1)
        {
            switch(cmd)
            {
            case 1: //查单词
                do_catchwords(sockfd);
                break;
            case 2: //查历史
                catch_history();
                break;
            case 3: //退出
                exit(EXIT_SUCCESS);
                break;
            default:printf("Input error!!!\n");
            }
        }else{
            printf("Input error!!!\n");
        }
    }
}


void catch_history()
{
    char buf[1024];

    FILE *fp = fopen(LOG_PATH, "r");
    if(fp == NULL)
    {
        perror("Failed to fopen");
        return ;
    }

    while(1)
    {
        if(fgets(buf, sizeof(buf)-1, fp) != NULL)
        {
            buf[strlen(buf)] = '\0';
            printf("%s",buf);
        }else{
            break;
        }
    }
}







