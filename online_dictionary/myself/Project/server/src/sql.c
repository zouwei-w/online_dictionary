#include "comm.h"

int exec_sql(char *database, char *sql, char **word_mean, int type)
{
    int ret;
    int nrow;
    int ncolumn;
    sqlite3 *pdb;
    char *errmsp;
    char **result;
    char buf[1024];

    ret = sqlite3_open(database, &pdb);
    if(ret < 0)
    {    
        fprintf(stderr,"Failed to sqlite3_open: %s\n",sqlite3_errmsg(pdb));
        return -1;
    }

    ret = sqlite3_get_table(pdb, sql, &result, &nrow, &ncolumn, &errmsp);
    if(ret != SQLITE_OK)
    {
        fprintf(stderr,"Failed to sqlite3_get_table:%s\n",errmsp);
        sqlite3_free(errmsp);
        return -1;
    }
    
    if(type == 1 && nrow == 1) //���ʵĺ���
    {
        strcpy(buf, result[ncolumn + 1]);
        *word_mean = buf;
    }

    return nrow;
}


int user_register(char *user_name, char *pass_word)
{
    int ret;
    char sql[1024];
    sprintf(sql,"select * from user_table where name='%s';",user_name);
    ret = exec_sql(DATABASE, sql, NULL, 2);
    if(ret == 1)
        return 2; //�Ѿ�ע��
    
    sprintf(sql,"insert into user_table values('%s','%s');",user_name,pass_word);
    ret = exec_sql(DATABASE, sql, NULL, 2);
    if(ret < 0)
        return -1; //ע��ʧ��

    return 0;  //ע��ɹ�
}

int user_login(char *user_name, char *pass_word)
{
    int ret;
    char sql[1024];
    sprintf(sql,"select * from user_table where name='%s' and password='%s';",user_name,pass_word);
    ret = exec_sql(DATABASE, sql, NULL, 2);
    if(ret == 1)
        return 1;  //��¼�ɹ�
    else
        return -1; //��¼ʧ��
}

char *find_words(char *word_s)
{
    int ret;
    char sql[1024];
    char *word_mean;
    sprintf(sql,"select * from word_table where word='%s';",word_s);
    ret = exec_sql(DATABASE, sql, &word_mean, 1);
    if(ret == 1)
        return word_mean; //�ҵ��˵���
    else
        return NULL;   //ʧ��
}

