#include "comm.h"

//a.out 192.168.1.108 7878
void *listen_function(void *p)
{
    int ret;
    mhead_t head_t;
    mhead_t msg_send;
    char *word_mean;
    int connect_fd = *(int *)p;

    free(p);
    while(1)
    {
        ret = recv(connect_fd, &head_t, sizeof(mhead_t), 0);
        if(ret <= 0)
        {
            perror("Failed to recv");
            close(connect_fd);
            pthread_exit(NULL);
        }

        switch(head_t.type)
        {
        case USER_REGISTER: //注册消息
            ret = user_register(head_t.username, head_t.password);
            if(ret == 2){ 
                msg_send.type = USER_HADREG;
                printf("had register\n");
            } //已经注册
            else if(ret == 0){ 
                msg_send.type = USER_SUCCESS;
                printf("regist success\n");
            } //注册成功
            else{ 
                msg_send.type = USER_FAILURE;
                printf("register failed\n");
            } //注册失败
            send(connect_fd, &msg_send, sizeof(mhead_t), 0);
            break;

        case USER_LOGIN:    //登录消息
            ret = user_login(head_t.username,head_t.password);
            if(ret == 1){
                msg_send.type = USER_SUCCESS;
                printf("login success\n");
            } //登录成功
            else{
                msg_send.type = USER_FAILURE;
                printf("login failed\n");
            }//登录失败
            send(connect_fd, &msg_send, sizeof(mhead_t), 0);
            break;

        case USER_WORD:     //查询单词消息
            printf("word:%s\n",head_t.word);
            word_mean = find_words(head_t.word);
            if(word_mean == NULL){     //查询失败
                msg_send.type = USER_FAILURE;
                strcpy(msg_send.word,"Don't have this word!");
                send(connect_fd, &msg_send, sizeof(mhead_t), 0);
                printf("Don't have this word!\n");
                break;
            } 
            else{   //查询成功
                msg_send.type = USER_SUCCESS;
                msg_send.size = strlen(word_mean);
                send(connect_fd, &msg_send, sizeof(mhead_t), 0);
                send(connect_fd, word_mean, strlen(word_mean), 0);
                printf("word_mean:%s\n",word_mean);
                break;
            } 
        }

    }

    close(connect_fd);
    pthread_exit(NULL);
}


int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        fprintf(stdout,"Usage : %s <IP> <Port>\n",argv[0]);
        return -1;
    }

    int ret;
    int listen_fd;
    int *pconnect_fd;
    pthread_t thread1;
    struct sockaddr_in myaddr;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0)
    {
        perror("Failed to socket");
        return -1;
    }
    printf("sockfd: %d\n",listen_fd);

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[2]));
    myaddr.sin_addr.s_addr = inet_addr(argv[1]);

    ret = bind(listen_fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if(ret < 0)
    {
        perror("Failed to bind");
        return -1;
    }

    ret = listen(listen_fd, 128);
    if(ret < 0)
    {
        perror("Failed to listen");
        return -1;
    }
    printf("Listen....\n");
    
    while(1)
    {   
        pconnect_fd = (int *)malloc(sizeof(int));
        *pconnect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
        if(*pconnect_fd < 0)
        {
            perror("Failed to accept");
            return -1;
        }

        ret = pthread_create(&thread1, NULL, listen_function, pconnect_fd);
        if(ret != 0)
        {
            perror("Failed to create pthread");
            return -1;
        }

        pthread_detach(thread1);
    }

    return 0;
}
