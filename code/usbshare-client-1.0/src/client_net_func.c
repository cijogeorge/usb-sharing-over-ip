
#include "../inc/usbshare_client.h"

void send_msg(int sockfd, char * sbuf)
{
 if(send(sockfd, sbuf, MAXDATASIZE-1 , 0) == -1)
 perror("send_msg()");
}

char * recv_msg(int sockfd)
{
 int numbytes;
 if((numbytes = recv(sockfd, rbuf, MAXDATASIZE-1, 0)) == -1)
 { 
  perror("recv_msg()");
  exit(1);
 }
 else  
 {
  rbuf[numbytes] = '\0';
  return rbuf;
 }
}

int con_to_serv(char *host)
{
 int sockfd;
 struct hostent *he;
 // connector’s address information
 struct sockaddr_in their_addr;
 // get the host info
 if((he=gethostbyname(host)) == NULL)
 {
  perror("gethostbyname()");
  exit(1);
 }
 else
 if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
  perror("socket()");
  exit(1);
 }
 // host byte order
 their_addr.sin_family = AF_INET;
 // short, network byte order
 their_addr.sin_port = htons(PORT);
 their_addr.sin_addr = *((struct in_addr *)he->h_addr);
 // zero the rest of the struct
 memset(&(their_addr.sin_zero), '\0', 8);
 if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
 {
  perror("connect()");
  exit(1);
 }
 else return sockfd;
}


