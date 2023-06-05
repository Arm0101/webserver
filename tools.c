#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "tools.h"

#define BUFFER_SIZE 2048

int type(const char* route){
    struct stat st;
    if (stat(route,&st) == 0){
        if (S_ISDIR(st.st_mode)) return IS_DIR;// es un directorio
        if (S_ISREG(st.st_mode)) return IS_FILE;// es un archivo
    }
    return NOT_EXIST; //no existe
}

char * read_file (char * route, size_t * length){
   //abrir archivo en modo lectura
    FILE *file = fopen(route,"r");
    if (file == NULL) {
        perror("file: ");
        return NULL;
    }
    char buff[BUFFER_SIZE];
    memset(buff,'\0',BUFFER_SIZE);
    size_t bytes_read = fread(buff,sizeof(char),BUFFER_SIZE,file);
    *length = bytes_read;
    //cerrar el archivo
    fclose(file);    
    return strdup(buff);
}

char* get_full_route(char* root,char *rel){
    char route[strlen(root) + strlen(rel)];
    sprintf(route,"%s%s",root,rel);
    return strdup(route);

}