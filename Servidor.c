#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

//Bibliotecas acrescentadas para cálculo do RTT ou taxa de conexão do servidor em ms
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define PORT 8080

struct Clientes_conectados{
    int posicao;
    char *host;
    double velocidade_conexao;
    struct clientes_conectados *prox;
};

void mostraClientes(){

    printf("+----------------------------------------------------------------------------------------------+\n");
    printf("|Endereço cliente|                          Velocidade de atendimento                          |\n");
    printf("+----------------+-----------------------------------------------------------------------------+\n");
    printf("|                |                                                                             |\n");
    printf("|                |                                                                             |\n");
    printf("+----------------------------------------------------------------------------------------------+\n");

}

struct Clientes_conectados *Clientes;

void Adiciona_cliente(char *host, double velocidade_conexao){
    struct Clientes_conectados *aux;
    aux = malloc(sizeof(struct Clientes_conectados));
    aux->host = (char*) host;
    aux->velocidade_conexao = velocidade_conexao;

    if(Clientes == NULL){
        Clientes = aux;
        printf("\nGravou o primeiro cliente\n");
    }else{
        Clientes->prox = (struct Clientes_conectados*) aux;
        printf("\nGravou o segundo cliente\n");

    }
    
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int status;

    // Creating socket file descriptor - AF_INET = IPv4, SOCK_STREAM = Protocolo de camada de transporte TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; //IPv4
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("\nAguardando conexão na porta %i...\n", PORT);
   
    //Adiciona_cliente("teste1", 2000);
    //Adiciona_cliente("teste2", 3000);
    //Adiciona_cliente("teste3", 4000);


    //printf("\n%s\n", Clientes->host);
    //Clientes = Clientes->prox;
    //printf("\n%s\n", Clientes->host);
    //Clientes = Clientes->prox;
    //printf("\n%s\n", Clientes->host);
    //Clientes = Clientes->prox;

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1024);

        

        printf("%s\n", buffer);

        mostraClientes();

        send(new_socket, "HTTP/1.0 200 OK\n\n<html><body><h1>Irio</h1><h1>testeaaa</h1></body></html>", strlen("HTTP/1.0 200 OK\n\n<html><body><h1>Irio</h1><h1>testeaaa</h1></body></html>"), 0);

        status = close(new_socket);
    }
    return 0;
}