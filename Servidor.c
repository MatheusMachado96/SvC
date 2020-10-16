#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>

int main(int argc, char **argv){   
    int s, client_s;
    struct sockaddr_in self, client;
    int addrlen = sizeof(client);
    char msg_write[100], msg_read[100];
    s = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&self, sizeof(self));
    self.sin_family = AF_INET;
    self.sin_port = htons(9999);
    self.sin_addr.s_addr = INADDR_ANY;
    bind(s, (struct sockaddr*)&self, sizeof(self));
    listen(s, 5);
    while (1) {
        client_s = accept(s, (struct sockaddr*)&client, &addrlen);
        do {
            read (client_s, msg_read, MAXBUF);
            write (client_s, msg_read, strlen(msg_read)+1);
        } while (strcmp(msg_read,"bye"));
            close(client_s);
    }
}