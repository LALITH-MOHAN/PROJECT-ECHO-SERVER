#ifndef MESSAGE_H
#define MESSAGE_H

struct message 
{
    char msg[1024];  // message
    char time[10];   // time stamp
    char date[11];   // present date
    int ct;          // count for number of messages echoed
    int uptime;      // uptime in seconds
};

#endif 
