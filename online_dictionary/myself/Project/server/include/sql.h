#ifndef _SQL_H
#define _SQL_H

#include "comm.h"

int exec_sql(char *database, char *sql, char **word_mean, int type);

int user_register(char *user_name, char *pass_word);

int user_login(char *user_name, char *pass_word);

char *find_words(char *word_s);




#endif
