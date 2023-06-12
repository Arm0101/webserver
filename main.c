#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include "server.h"
#include "response.h"
#include "tools.h"

#define MAX_CLIENTS 10

void* handle_client(void* arg) {
    int client_fd = *(int*)arg;
    handle_request(client_fd);
    close(client_fd);
    pthread_exit(NULL);
}

int main(int argc,char** args) {

    signal(SIGPIPE,SIG_IGN);
    
    char current_dir[1024];
    getcwd(current_dir,1024);
    int port = 1234;
    char* dir = "/home";
    
    //establecer puerto y directorio de los argumentos
    if (argc > 1){
       
       int _port;
       if ((_port = atoi(args[1])) != 0 ) port = _port;

       if (args[2] != NULL) dir = args[2];
    }

    int server_fd ,client_fd;
    server_fd = init_server(port, dir, current_dir); //iniciar servidor
    if (server_fd < 0){
        printf("Error al iniciar el servidor");
        return -1;
    }

    int max_fd = server_fd;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    int client_fds[MAX_CLIENTS];
    int num_clients = 0;

    while (1)
    {
        fd_set tmp_fds = read_fds;
        int result = select(max_fd + 1, &tmp_fds, NULL, NULL, NULL); //esperar por actividad en los sockets del conjunto
        if (result < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == server_fd) {
                    client_fd = wait_client(server_fd); //esperar por conexion del cliente
                    if (num_clients < MAX_CLIENTS) {
                        client_fds[num_clients] = client_fd;
                        num_clients++;
                        FD_SET(client_fd, &read_fds); //agregar el nuevo socket de cliente al conjunto
                        if (client_fd > max_fd) {
                            max_fd = client_fd;
                        }
                    } else {
                        printf("Demasiados clientes conectadosn");
                        close(client_fd);
                    }
                } else {
                    for (int j = 0; j < num_clients; j++) {
                        if (client_fds[j] == i) {
                            pthread_t thread;
                            int* arg = malloc(sizeof(*arg));
                            *arg = i;
                            pthread_create(&thread, NULL, handle_client, arg); //manejar la solicitud correspondiente en un hilo separado
                            pthread_detach(thread);
                            FD_CLR(i, &read_fds); //eliminar el socket de cliente del conjunto
                            for (int k =j; k < num_clients - 1; k++) {
                                client_fds[k] = client_fds[k + 1];
                            }
                            num_clients--;
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}