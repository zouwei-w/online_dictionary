#include "comm.h"

FILE *getFile()
{
    FILE *fp;
    fp = fopen(LOG_PATH,"a+");
    if(fp == NULL)
    {
        fprintf(stdout,"fopen error: %s\n",strerror(errno));
        return NULL;
    }
    return fp;
}

int get_line(FILE *fp)
{
    int ch,count = 0;
    while((ch = fgetc(fp)) != EOF)
    {
        if(ch == '\n')
            count ++;
    }

    return count;
}

void do_log(char *exp_word)
{ 
    time_t currentTime;
    struct tm *tim;
    int year,month,day,hour,minutes,seconds;
    FILE *fp = getFile();
    int count = get_line(fp);

    currentTime = time(NULL);
    tim = localtime(&currentTime); 
    year = tim->tm_year + 1900;
    month = tim->tm_mon + 1;
    day = tim->tm_mday;
    hour = tim->tm_hour;
    minutes = tim->tm_min;
    seconds = tim->tm_sec;

    fprintf(fp,"[(%d) %d/%02d/%02d %02d:%02d:%02d]\t%s\n",++count,year, \
        month,day,hour,minutes,seconds,exp_word);

    fflush(fp);    

    fclose(fp);
}





