#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

struct Testclass
{
    char name[32];
    char saddr[250];
    int regno;
};

int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) 
    {
        perror("Socket creation failed");
        return 1;
    }
    printf("CLIENT_FD: %d\n", client_fd);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
    {
        perror("Connection failed");
        return 1;
    }

    struct Testclass sender;
    sender.regno = 80;
    strcpy(sender.name, "LALITH");
    strcpy(sender.saddr, "COIMBATORE");

    send(client_fd, (char*)&sender, sizeof(sender), 0);

    char buff[100] = {0};
    recv(client_fd, buff, sizeof(buff), 0);
    printf("\n%s\n", buff);

    close(client_fd);
}
