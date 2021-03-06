#include "comm.h"

#define MFULSH {char ch=0;while((ch=getchar()) != '\n' && ch != EOF);}

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


int Init_socket(const char *ip, const char *port)
{
    int ret;
    int sockfd;
    struct sockaddr_in client_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Failed to socket");
        return -1;
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(port));
    client_addr.sin_addr.s_addr = inet_addr(ip);

    ret = bind(sockfd,(struct sockaddr *)&client_addr, sizeof(client_addr));
    if(ret < 0)
    {
        perror("Failed to bind");
        return -1;
    }

    return sockfd;
}

int send_message(int sockfd, char *server_ip, int server_port)
{
    int ret;
    int cmd;
    struct sockaddr_in server_addr;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((short)server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0)
    {
        perror("Failed to connect");
        return -1;
    }
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
    char server_ip[16];
    int server_port;
    int sockfd = *(int *)p;
    while(1)
    {
        printf("Please input server_ip: ");
        fgets(server_ip, sizeof(server_ip)-1, stdin);
        if(server_ip[strlen(server_ip) - 1] == '\n')
            server_ip[strlen(server_ip) - 1] = '\0';
        printf("Please input server_port: ");
        scanf("%d",&server_port);MFULSH;
        

        send_message(sockfd, server_ip, server_port);
    }

    pthread_exit(NULL);
}



int main(int argc, const char *argv[])
{
    int ret;
    int sockfd;
    pthread_t thread1;

    if(argc < 3)
    {
        printf("Usage : %s <client_ip> <client_port>\n",argv[0]);
        return -1;
    }

    sockfd = Init_socket(argv[1],argv[2]); //初始化套接字
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







