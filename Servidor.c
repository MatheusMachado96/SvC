#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#include "lib.h"

#include <fcntl.h>
#include <pthread.h> //biblioteca de threads em c

//Bibliotecas acrescentadas para cálculo do RTT ou taxa de conexão do servidor em ms
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <regex.h>

#include <sys/stat.h>

#define PORT 8080

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

struct Clientes_conectados
{
    int posicao;
    char *host;
    double velocidade_conexao;
    struct clientes_conectados *prox;
};

typedef struct
{
    int new_socket;
} thread_arg, *ptr_thread_arg;

int throughput_restante = 1000;

void serve_file(int, const char *);

void execute_cgi(int, const char *, const char *, const char *);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_conection(void *p_client_socket);

int get_line(int, char *, int);

void mostraClientes()
{

    printf("+----------------------------------------------------------------------------------------------+\n");
    printf("|Endereço cliente|                          Velocidade de atendimento                          |\n");
    printf("+----------------+-----------------------------------------------------------------------------+\n");
    printf("|                |                                                                             |\n");
    //printf("|                |                                                                             |\n");
    printf("+----------------------------------------------------------------------------------------------+\n");
}

struct Clientes_conectados *Clientes;

void Adiciona_cliente(char *host, double velocidade_conexao)
{
    struct Clientes_conectados *aux;
    aux = malloc(sizeof(struct Clientes_conectados));
    aux->host = (char *)host;
    aux->velocidade_conexao = velocidade_conexao;

    if (Clientes == NULL)
    {
        Clientes = aux;
        printf("\nGravou o primeiro cliente\n");
    }
    else
    {
        Clientes->prox = (struct Clientes_conectados *)aux;
        printf("\nGravou o segundo cliente\n");
    }
}

void er_match(char *argv[])
{
	/* aloca espaço para a estrutura do tipo regex_t */
	regex_t reg;

	/* compila a ER passada em argv[1]
	 * em caso de erro, a função retorna diferente de zero */
	if (regcomp(&reg , argv[1], REG_EXTENDED|REG_NOSUB) != 0) {
		fprintf(stderr,"erro regcomp\n");
		exit(1);
	}
	/* tenta casar a ER compilada (&reg) com a entrada (argv[2]) 
	 * se a função regexec retornar 0 casou, caso contrário não */
	if ((regexec(&reg, argv[2], 0, (regmatch_t *)NULL, 0)) == 0)
		printf("Casou\n");
	else
		printf("Não Casou\n");
}

int get_line(int sock, char *buf, int size)
{
 int i = 0;
 char c = '\0';
 int n;

 while ((i < size - 1) && (c != '\n'))
 {
  n = recv(sock, &c, 1, 0);
  /* DEBUG printf("%02X\n", c); */
  if (n > 0)
  {
   if (c == '\r')
   {
    n = recv(sock, &c, 1, MSG_PEEK);
    /* DEBUG printf("%02X\n", c); */
    if ((n > 0) && (c == '\n'))
     recv(sock, &c, 1, 0);
    else
     c = '\n';
   }
   buf[i] = c;
   i++;
  }
  else
   c = '\n';
 }
 buf[i] = '\0';
 
 return(i);
}

void serve_file(int client, const char *filename)
{
 FILE *resource = NULL;
 int numchars = 1;
 char buf[1024];

 printf("\nEntrou no serve file\n");
/*
 buf[0] = 'A'; buf[1] = '\0';
 while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
 /*
  numchars = get_line(client, buf, sizeof(buf));

 
 resource = fopen(filename, "r");

 printf("\n%s\n", filename);
 if (resource == NULL)
  not_found(client);
 else
 {
  headers(client, filename);
  cat(client, resource);
 }
 */

 sprintf(buf, "HTTP/1.0 200 NOT FOUND\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "your request because the resource specified\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "is unavailable or nonexistent.\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "</BODY></HTML>\r\n");
 send(client, buf, strlen(buf), 0);


 //fclose(resource);
}

void headers(int client, const char *filename)
{
 char buf[1024];
 (void)filename;  /* could use filename to determine file type */

 
 /*
 strcpy(buf, "HTTP/1.0 200 OK\r\n");
 send(client, buf, strlen(buf), 0);
 strcpy(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 strcpy(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
    
*/
   


}



void not_found(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, SERVER_STRING);
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Type: text/html\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "Content-Length: 164\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "\r\n");
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n"); //32
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "<BODY><P>The server could not fulfill\r\n"); //39
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "your request because the resource specified\r\n"); //45
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "is unavailable or nonexistent.\r\n"); //32
 send(client, buf, strlen(buf), 0);
 sprintf(buf, "</BODY></HTML>\r\n"); //16
 send(client, buf, strlen(buf), 0);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int status;
    int i = NULL;
    pthread_t threads[20000];
    thread_arg argumentos[20000];

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

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t t;
        
        argumentos[i].new_socket = new_socket;

        //pclient = new_socket;
        pthread_create(&t, NULL, handle_conection, &(argumentos[i]));
        i++;
    }
    return 0;
}

void *handle_conection(void *p_client_socket)
{
    int client_socket = *((int *)p_client_socket);
    int valread, status;
    int numchars;
    int i, j;
    int cgi = 0;

    char buffer[1024] = {0};
    char buffer_novo[1] = {0};
    char buffer_2[1024] = {0};
    char method[255];
    char url[255];
    char path[512];
    char ch;
    char *query_string = NULL;
    char *token;
    char *token2;
    char *nome_arquivo_html;
    char *nome_imagem;
    char Linha[100];
    char *result;
    char tamanho_char[50];

    struct stat st;

    long tamanho;

    ptr_thread_arg targ = (ptr_thread_arg)p_client_socket;

    FILE *fp;
    FILE *fp2;

    clock_t Ticks[2];
    
    char *raw_request = {0};
   
    //char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 83\n\n<html><head><title>teste</title></head><body><img src='download.png'></body></html>";
    char *header_html = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 83\n\n";
    char *header_html3 = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    char *header_html4 = "\n\n";
    char imageheader[] =
        "HTTP/1.1 200 Ok\r\n"
        "Content-Type: image/png\r\n"
        "Content-Length: 621378\r\n\r\n";

    
   

    while (1)
    {
       
        valread = read(targ->new_socket, buffer, 1024);

        if (valread == 0){
            close(targ->new_socket);
            pthread_exit(0);
        }

        raw_request = buffer;
        struct Request *req = parse_request(raw_request);
        struct Header *h;

        if (req) {
            printf("Method: %d\n", req->method);
            printf("Request-URI: %s\n", req->url);
            printf("HTTP-Version: %s\n", req->version);
            puts("Headers:");
            
            for (h=req->headers; h; h=h->next) {
                //printf("%32s: %s\n", h->name, h->value);
                //Faz envio de HTML
                if(strcmp(h->name, "Sec-Fetch-Dest") == 0 && strcmp(h->value, "document") == 0){
                    fp2 = fopen(strtok(req->url, "/"), "r");
                    if(fp2 == NULL){
                        printf("\nArquivo não encontrado\n");
                        not_found(targ->new_socket);
                        pthread_exit(0);
                    } 
                    if (fp2 != NULL) {
                        // movimenta a posição corrente de leitura no arquivo 
                        // para o seu fim
                        fseek(fp2, 0, SEEK_END);
                        // pega a posição corrente de leitura no arquivo
                        tamanho = ftell(fp2);
                        // imprime o tamanho do arquivo
                        printf("O arquivo possui %ld bytes", tamanho);
                        fseek(fp2, 0, SEEK_SET);
                    } 
                    Ticks[0] = clock();

                    //tamanho_char = itoa(tamanho, tamanho_char,10);
                    int n = sprintf(tamanho_char, "teste %d", tamanho);
                    printf("\n%s %d %d\n", tamanho_char, strlen(tamanho_char), strlen("83"));
                    write(targ->new_socket, header_html3, strlen(header_html3));
                    write(targ->new_socket, "83", strlen("83"));
                    write(targ->new_socket, header_html4, strlen(header_html4));
                    
                    //write(targ->new_socket, header_html, strlen(header_html));
                    
                    while(!feof(fp2)){
                        result = fgets(Linha, 100, fp2); 
                        printf("\n%s\n", Linha);
                        write(targ->new_socket, Linha, strlen(Linha));
                    }
                    //write(targ->new_socket, hello, strlen(hello));
                    Ticks[1] = clock();
                    double Tempo = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
                    printf("\nTempo RTT gasto: %g s.", Tempo);
                    printf("\nVazão de %g KBps.\n", tamanho / (Tempo * 1024));
                    //início seção crítica
                    pthread_mutex_lock(&mutex);
                    printf("\nEntrou na seção crítica\n");
                    throughput_restante -= 250;
                    pthread_mutex_unlock(&mutex);
                    //fim seção crítica
                }
                //Faz envio de imagens
                if(strcmp(h->name, "Sec-Fetch-Dest") == 0 && strcmp(h->value, "image") == 0){
                   //if (!strncmp(buffer, "GET /download.png", 17)){
                        // Send header first: (Note, should loop in case of partial write or EINTR)
                        write(targ->new_socket, imageheader, sizeof(imageheader) - 1);
                        // Send image content
                        fp = open(strtok(req->url, "/"), O_RDONLY);
                        int sent = sendfile(targ->new_socket, fp, NULL, 700000);
                        printf("\nbytes enviados: %d\n", sent);
                        close(fp);
                    //}
                }
            }
            puts("message-body:");
            puts(req->body);
        }
        
        free_request(req);
    }

    close(targ->new_socket);
    pthread_exit(0);
}