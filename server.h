#ifndef SERVER_H
#define SERVER_H

int init_server(unsigned int, const char *, const char*);
int wait_client(int);
void handle_request(int);
#endif