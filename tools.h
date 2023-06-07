
#include <unistd.h>
#ifndef TOOLS_H
#define TOOLS_H

enum {IS_FILE,IS_DIR,NOT_EXIST};
char * read_file(char *,size_t*);
int type (const char* ); //para saber si es un archivo o directorio;
char* get_full_route(const char* ,const char *);
#endif