#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"
#include "response.h"
#include "tools.h"

int main(int argc,char** args) {

    signal(SIGPIPE,SIG_IGN);
    
    int port = 1234;
    char* dir = "/home";
    
    //establecer puerto y directorio de los argumentos
    if (argc > 1){
       
       int _port;
       if ((_port = atoi(args[1])) != 0 ) port = _port;

       if (args[2] != NULL) dir = args[2];
    }

    int server_fd ,client_fd;
    server_fd = init_server(port, dir); //iniciar servidor
    while (1)
    {
        client_fd = wait_client(server_fd); //esperar por conexion del cliente
        handle_request(client_fd);
        close(client_fd);
        
    }
    return 0;
}