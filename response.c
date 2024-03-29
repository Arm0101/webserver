#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "response.h"
#include "tools.h"


#define BUFFER_SIZE 4096
#define NOT_FOUND_FILE "response_files/404.html" 
#define FORBIDDEN_FILE "response_files/403.html" 
#define RESPONSE_FILE "response_files/response.html"
#define RESPONSE_BODY_FILE "response_files/response_body.html"


char* fill_table(const char*,const char*,const char*, const char*, size_t*);
char* get_href(const char*,const char*,const char*);


char* get_header(char * status, char* message, char* content_type, size_t content_length, char* disposition){
    char* _header ="HTTP/1.1 %s %s\r\n Content-Type: %s \r\n Content-length: %ld\r\n Content-Disposition: %s\r\n\r\n";
    char header[BUFFER_SIZE];
    memset(header,'\0', BUFFER_SIZE);
    sprintf(header,_header,status,message,content_type,content_length,disposition);
    return strdup(header); 
}

void resp_not_found(int client_fd){
    char * content;
    size_t content_length;
    content = read_file(NOT_FOUND_FILE,&content_length);
    char content_type [] = "text/html";
    char *header = get_header("404", "NOT FOUND", content_type,strlen(content), "inline");

    send(client_fd,header,strlen(header),0);
    send(client_fd,content,content_length,0);
    free(header);
    free(content);
}

void resp_forbidden(int client_fd){
    char * content;
    size_t content_length;
    content = read_file(FORBIDDEN_FILE,&content_length);
    char content_type [] = "text/html";
    char *header = get_header("403", "Forbidden", content_type,strlen(content), "inline");

    send(client_fd,header,strlen(header),0);
    send(client_fd,content,content_length,0);
    free(header);
    free(content);
}
void send_file(int client_fd, const char* route){
 
    //abrir archivo en modo lectura
    int file = open(route,O_RDONLY);
    if (file == -1) {
        perror("file: ");
        return NULL;
    }

    //obtener el tamano del archivo
    struct stat st; 
    stat(route,&st);
    size_t size = st.st_size;

    char* header = get_header("200","OK","application/octet-stream",size,"attachment");
    write(client_fd,header,strlen(header));
    free(header);
    size_t chunk_size = 2048*1024;
    off_t offset = 0;
    while (offset < size)
    {
       size_t to_send = size - offset;
       if ( to_send > chunk_size) to_send = chunk_size;
       if (sendfile(client_fd, file, &offset, to_send) == -1)
        perror("sendfile: ");
    }
    //cerrar el archivo
    close(file);
}



void send_content(int client_fd,const char* relative_route, const char* route){
    size_t cont_length;
    char* _content = read_file(RESPONSE_FILE,&cont_length);
    
    char content[cont_length + strlen(route) + 2];
    memset(content,'\0',cont_length + strlen(route));
    sprintf(content,_content,route);


    char* header = get_header("200","OK","text/html",0,"inline");
    send(client_fd,header,strlen(header),0);
    
    char* resp1 = strtok(content,"$");
    send(client_fd,resp1,strlen(resp1),0);
    

    //enviar cuerpo de la tabla
    size_t table_length;
    char* table = read_file(RESPONSE_BODY_FILE,&table_length);

    DIR* dir = opendir(route);
    struct dirent *dirp;
    
    dirp = readdir(dir);
    while (dirp != NULL)
    {   
        size_t len;
        char* d = fill_table(table, relative_route, route, dirp->d_name,&len);
        //enviar al cliente
        send(client_fd,d,len,0);
        free(d);
        dirp = readdir(dir);
    }

    resp1 = strtok(NULL,"$");
    send(client_fd,resp1,strlen(resp1),0);
    
     closedir(dir);
     free(header);
     free(table);
     free(_content);
}

char* fill_table(const char* table,const char* relative_route,const char* route, const char* name, size_t* size){
    char * href = get_href(relative_route,route,name);
    size_t len = strlen(table) + strlen(href) + strlen(name);
    char col[len];
    memset(col,'\0',len);
    sprintf(col,table,href,name);
    *size = strlen(col);
    free(href);
    return strdup(col);
}


char* get_href(const char* relative_route ,const char *route,const char* name){
    int len = strlen(relative_route) + strlen(name) + 2;
    char _href[len];
    memset(_href,'\0', len);
    char* aux = get_full_route(route,name);
    if (type(aux) == IS_FILE){
        sprintf(_href, "%s%s",relative_route,name);
        free(aux);
        return strdup(_href);
    }

    sprintf(_href, "%s%s/",relative_route,name);
    free(aux);
    return strdup(_href);
}