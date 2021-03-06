#include "../inc/usbshare_client.h"

/* ITOA */

char* itoa(int val, int base)
{
 static char buf[32] = {0};
 int i = 30;
 for(; val && i ; --i, val /= base)
 buf[i] = "0123456789abcdef"[val % base];
 return &buf[i+1];
}

/* MAKEDIR */

int makedir(char *path)
{
 int ret;
 static mode_t mode= 0755;
 ret=mkdir(path, mode);
 if(ret) perror("mkdir()"); 
 return ret;
}

/* RMVDIR */

int rmvdir(char *dirname)
{
 DIR *dirc;
 struct dirent *drd;
 int ret;
 static int flag=0;
 struct dirent *entry;
 char path[PATH_MAX];
 dirc = opendir(dirname);
 while ((entry = readdir(dirc)) != NULL) 
 {
  if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
  {
   snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
   if (entry->d_type == DT_DIR) rmvdir(path);
   remove(path);
  }
 }
 closedir(dirc);
 ret=rmdir(dirname);
 if(!ret)
 {
  return 0;
  flag=0;
 }
}

/* CP FILE FROM */

int cpfilefrom(char *file, char *dirname)
{
 int i, numbytes;
 char ch;
 char *path=(char *) malloc(strlen(dirname)+strlen(file)+1);
 sprintf(path,"%s%s",dirname,file);
 FILE * fp;
 if(!(fp=fopen(path,"wb")))
 {
  perror("fopen()"); 
  return 1;
 }
 else
 {
  strcpy(rbuf,recv_msg(sockfd));
  int file_size=atoi(rbuf);
  for(i=0;i<file_size;i++)
  { 
   if((numbytes = recv(sockfd, &ch, 1, 0)) == -1)
   { 
    perror("recv()");
    return(1);
   }
   else 
    putc(ch,fp);
  }
  fclose(fp);
  strcpy(rbuf,recv_msg(sockfd));
  mode_t mode=atoi(rbuf);
  if(chmod(path, mode)) 
  {
   perror("fchmod()");
   return(1);
  }
 }
 return 0;
}

/* MV FILE FROM */

int mvfilefrom(char *file, char *dirname)
{
 int ret = cpfilefrom(file,dirname);
 return ret;
}

/* CP FILE TO */

int cpfileto(char *path)
{
 char byte;
 FILE * fp;
 if(fp=fopen(path,"rb"))
 {
  int type,i; 
  struct stat statbuff;
  fseek(fp, 0L, SEEK_END);
  int file_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  strcpy(sbuf,itoa(file_size,10));
  send_msg(sockfd, sbuf);
  for(i=0;i<file_size;i++)
  {
   byte=getc(fp);
   if(feof(fp)) break;
   else
   {
    if(send(sockfd, &byte, 1, 0) == -1)
    { perror("send()"); return 1; }
   }
  }
  type =lstat(path, &statbuff);
  if(type!=0)
   { perror("lstat()"); return 1; }
  else 
  { 
   strcpy(sbuf,itoa(statbuff.st_mode,10));
   send_msg(sockfd, sbuf);
  }
  return 0;
 }
 else return 1;
}

/* MV FILE TO */

int mvfileto(char *path)
{
 int ret = cpfileto(path);
 unlink(path);
 return ret;
}

/* CP DIR FROM */

int cpdirfrom(char *path)
{
 int ret;
 char dirname[50], filename[50], path1[100];
 while(1)
 {
  strcpy(rbuf,recv_msg(sockfd));
  if(!strcmp("D",rbuf))
  {
   strcpy(dirname, recv_msg(sockfd));
   sprintf(path1,"%s%s/",path,dirname);
   if(makedir(path1)) return 1;
   ret=cpdirfrom(path1);
   if(ret) return 1;
  }
  else if(!strcmp("R",rbuf))
  {
   strcpy(filename, recv_msg(sockfd));
   ret=cpfilefrom(filename, path);
   if(ret) return 1;
  }
  else if(!strcmp("E", rbuf)) return 0;  
 }
 return 0;
}

/* MV DIR FROM */

int mvdirfrom(char * path)
{
 int ret = cpdirfrom(path);
 return ret;
}

/* CP DIR TO */

int cpdirto(char * path)
{
 int ret;
 DIR *dop;
 struct dirent *drd;
 if ((dop = opendir(path)) == NULL)
 perror("opendir()");
 while ((drd = readdir(dop)) != NULL)
 {
  if(strcmp(".",drd->d_name)!=0 && strcmp("..",drd->d_name)!=0)
  {
   int type; 
   struct stat statbuff;
   char path1[100];
   sprintf(path1,"%s%s",path,drd->d_name);
   type =lstat(path1, &statbuff);
   if(type!=0)
    perror("lstat()");
   else
   { 
    if((statbuff.st_mode&S_IFMT)==S_IFDIR)
    {
     send_msg(sockfd,"D");
     send_msg(sockfd,drd->d_name);
     sprintf(path1,"%s%s/",path,drd->d_name);
     ret=cpdirto(path1);
     if(ret) return 1;
    }
    else if((statbuff.st_mode&S_IFMT)==S_IFREG)
    {
     send_msg(sockfd,"R"); 
     send_msg(sockfd, drd->d_name);
     sprintf(path1,"%s%s",path,drd->d_name);
     ret=cpfileto(path1);
     if(ret) return 1;
    }
   }
  }
 }
 send_msg(sockfd,"E");
 return 0;
}

/* MV DIR TO */

int mvdirto(char *path)
{
 int ret = cpdirto(path);
 if(!ret) rmvdir(path);
 return ret;
}
