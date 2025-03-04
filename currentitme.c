#include<stdio.h>
#include<time.h>

int main()
{
    time_t current_time;
    struct tm *time_info;
    time(&current_time); //get current time
    time_info=localtime(&current_time);
    char buff[9];
    sprintf(buff,"%d:%d:%d",time_info->tm_hour,time_info->tm_min,time_info->tm_sec);
    printf("TIME:%s\n",buff);
    printf("TIME:%02d:%02d:%02d\n",time_info->tm_hour,time_info->tm_min,time_info->tm_sec);
}