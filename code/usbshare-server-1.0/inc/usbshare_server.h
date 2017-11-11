#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <sys/statvfs.h>
#include <sys/stat.h>

#define MYPORT 3490
#define BACKLOG 10
#define MAXDATASIZE 1000

int sock_setup();
void send_msg(int, char *);
char * recv_msg(int);
int accept_con(int);

char* itoa(int, int);
char *getdevlst();
char * list();
int makedir(char *);
int rmv(char *, int);
int copyfilefromusb(char *, int);
int copyfiletousb(char *, char *, int);
int copydirfromusb(char *, int);
int copydirtousb(char *, int);
int renamedir(char *, char *);
int movefiletousb(char*, char*, int);
int movefilefromusb(char*, int);
int movedirfromusb(char*, int);
int movedirtousb(char*, int);
int removedir(char *);
int chgdir(char *);
char * freespace();

char rbuf[MAXDATASIZE], sbuf[MAXDATASIZE], dir[MAXDATASIZE], device[50], tempdir[MAXDATASIZE], *devices[10];
int nodevices, retflag;
