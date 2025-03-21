#include "encrypt.h"
#include <string.h>

void encrp_decrp(char *msg, char *key) 
{
    int keyl = strlen(key);
    for (int i = 0; msg[i] != '\0'; i++) 
    {
        msg[i] ^= key[i % keyl];
    }
}
