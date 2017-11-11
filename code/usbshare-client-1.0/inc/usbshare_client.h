#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define PORT 3490
#define MAXDATASIZE 1000

void send_msg(int, char *);
char * recv_msg(int);
int con_to_serv(char *);

int cpfilefrom(char * ,char *);
int cpdirfrom(char *);
int cpdirto(char *);
int cpfileto(char *);
int rmvdir(char *);
int mvfileto(char *);
int mvfilefrom(char *, char *);
int mvdirfrom(char *);
int mvdirto(char *);
int makedir(char *);
char* itoa(int, int);

char rbuf[MAXDATASIZE], sbuf[MAXDATASIZE], dir[MAXDATASIZE], device[50], host[50];
int sockfd;
