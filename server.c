#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "response.h"
#include "tools.h"

#define SERVER_IP "127.0.0.1"
#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096

char INITIAL_ROUTE[256];



int init_server(unsigned int PORT,const char * route){
    int server_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int));

    // establecer direccion del servidor
     memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // asignar direccion al socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    //establecer ruta inicial
    strcpy(INITIAL_ROUTE,route);

    printf("Listening in port: %d \n", PORT);
    printf("Serving Directory: %s \n", INITIAL_ROUTE);


    return server_fd;
}

int wait_client(int server_fd){
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept");
            exit(1);
    }
    return client_fd;
}

char* get_route(char* req){

    char* token = strtok(req," ");
    token = strtok(NULL, " ");
    return strdup(token);
}


void handle_request(int client_fd){

        //leer la peticion del cliente    
        char req[BUFFER_SIZE];
        memset(req,'\0',BUFFER_SIZE);
        
        recv(client_fd, req,sizeof(req),0);
    
        // //obtener ruta
        if (!strncmp(req,"GET",3)){
            char *relative_route = get_route(req);
            char *route = get_full_route(INITIAL_ROUTE, relative_route);
            int t = type(route);

            //verificar si existe el archivo o directorio
            if (t == NOT_EXIST){
                resp_not_found(client_fd);
                return;
            }
            //verificar permisos
            if (access(route,R_OK) == -1) {
                resp_forbidden(client_fd);
                return;
            }

            if (t == IS_FILE){
                send_file(client_fd,route);
            }
            if (t == IS_DIR){
                size_t content_length;
                send_content(client_fd,relative_route,route,&content_length);
                //send_response(client_fd,"text/html",content,content_length);
                
            }
     
        }
        else {
         //HACER ALGO CON ESTO
         resp_not_found(client_fd);
        }
}


