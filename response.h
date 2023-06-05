#include <unistd.h>
#ifndef RESPONSE_H
#define RESPONSE_H

void resp_not_found(int);
void send_response(int,char*, char*, size_t);
void send_file(int,const char*);
void resp_forbidden(int);
void send_content(int,const char*,const char *, size_t*);
#endif