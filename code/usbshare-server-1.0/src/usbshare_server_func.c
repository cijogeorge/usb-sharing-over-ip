#include "../inc/usbshare_server.h"

/* ITOA */

char* itoa(int val, int base)
{
 static char buf[32] = {0};
 int i = 30;
 for(; val && i ; --i, val /= base)
 buf[i] = "0123456789abcdef"[val % base];
 return &buf[i+1];
}

/* CHKDIR */

int chkdir(char* path)
{
 int type; 
 struct stat statbuff;
 type =lstat(path, &statbuff);
 if(type!=0) 
 { 
  perror("lstat()"); 
  return 1; 
 }
 else if((statbuff.st_mode&S_IFMT)==S_IFDIR)
 return 0;
 else return 1;
} 

/* GET DEV LST */

char * getdevlst()
{
 int i, j=0;
 nodevices=0;
 char path[MAXDATASIZE];
 char *devlst=(char*) malloc(1);
 *devlst='\0';
 char buffer1[MAXDATASIZE], buffer2[MAXDATASIZE], buffer3[MAXDATASIZE];
 DIR *dop;
 struct dirent *drd;
 FILE *fp=fopen("/etc/mtab","r");
 if(!fp) perror("fopen()");
 if ((dop = opendir("/dev/disk/by-id")) == NULL)
 perror("opendir()");
 while ((drd = readdir(dop)) != NULL) 
 {
  for (i=0;i<=strlen(buffer1);i++) buffer1[i]='\0';
  if (strcmp(drd->d_name, ".") && strcmp(drd->d_name, "..")) 
  {
   if(strstr(drd->d_name, "usb") && strstr(drd->d_name, "part"))
   { 
    sprintf(path,"/dev/disk/by-id/%s", drd->d_name);
    if(readlink(path, buffer1, sizeof(buffer1)) < 0) perror("readlink()");
    else
    {
     i=strlen(buffer1);
     buffer1[i+1]='\0';
     while(buffer1[i--]!='/');
     strcpy(buffer1,&buffer1[i+2]);
     sprintf(buffer3,"/dev/%s",buffer1);
     fseek(fp, 0L, SEEK_SET);
     while(!feof(fp))
     {
      *buffer2='\0';
      fscanf(fp,"%s",buffer2);
      if(!strcmp(buffer3, buffer2))
      {
       fscanf(fp,"%s",buffer2); 
       sprintf(buffer2,"%s/",buffer2);
       devlst=realloc(devlst, strlen(devlst)+strlen(buffer2)+sizeof(j)+4);
       devices[j]=(char *)malloc(strlen(buffer2)+1); 
       strcpy(devices[j],buffer2);
       sprintf(devlst,"%s%d.\t%s\n", devlst, ++j, buffer2);
       break;
      }
     }
    }
   }
  }
 }
 nodevices=j;
 fclose(fp);
 closedir(dop);
 return devlst;
} 

/* LIST */

char * list()
{
 char *list=(char*) malloc(1);
 * list='\0';
 DIR *dop;
 struct dirent *drd;
 if ((dop = opendir(dir)) == NULL)
 {
  perror("opendir()");
  return "Invalid input!\n";
 }
 while ((drd = readdir(dop)) != NULL)
 {
  if(strcmp(".",drd->d_name)!=0 && strcmp("..",drd->d_name)!=0)
  {
   int type; 
   struct stat statbuff;
   char path[MAXDATASIZE];
   sprintf(path,"%s%s",dir,drd->d_name);
   type =lstat(path, &statbuff);
   if(type!=0)
   {
    perror("lstat()");
    return "Unsuccessful!\n";
   }
   else
   { 
    switch(statbuff.st_mode&S_IFMT)
    {
     case S_IFDIR : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                    sprintf(list,"%sDIR\t%s\n",list,drd->d_name);
                    break;
     case S_IFCHR : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                    sprintf(list,"%sCHR\t%s\n",list,drd->d_name);
                    break;   
     case S_IFREG : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                    sprintf(list,"%sREG\t%s\n",list,drd->d_name);
                    break;
     case S_IFBLK : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                    sprintf(list,"%sBLK\t%s\n",list,drd->d_name);
                    break;
     case S_IFSOCK : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                     sprintf(list,"%sSOC\t%s\n",list,drd->d_name);
                     break;
     case S_IFLNK : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
                    sprintf(list,"%sLNK\t%s\n",list,drd->d_name);
                    break;
     default : list=realloc(list, strlen(list)+strlen(drd->d_name)+7);
               sprintf(list,"%sUDF\t%s\n",list,drd->d_name);
               break;
    }
   }
  }
 }
 closedir(dop);
 return list;
}

/* FREESPACE */

char * freespace()
{
 char * usage=(char*) malloc(50);
 struct statvfs sbuf;
 if (statvfs(dir, &sbuf) < 0)
 perror("statvfs()");
 sprintf(usage,"Free Space on the disk: %ld MB\n",(sbuf.f_bsize*sbuf.f_bavail)/(1024*1024));
 return usage;
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

/* RMV */

int rmv(char *dirname, int new_fd)
{
 struct stat statbuff;
 int type =lstat(dirname, &statbuff);
 if(type!=0)
 perror("lstat()");
 else
 { 
  if((statbuff.st_mode&S_IFMT)!=S_IFDIR)
  {
   unlink(dirname);
   return 0;
  }
 }
 DIR *dirc;
 int ret;
 static int flag=0;
 struct dirent *entry;
 char path[PATH_MAX];
 dirc = opendir(dirname);
 if (dirc == NULL) return 1;

 while ((entry = readdir(dirc)) != NULL) 
 {
  if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
  {
   if(!flag)
   {
    send_msg(new_fd,"confirm");
    strcpy(rbuf,recv_msg(new_fd));

    if(strcmp("y",rbuf)) return 1;
    else 
     flag=1;
   }
   snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
   if (entry->d_type == DT_DIR) rmv(path, new_fd);
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

/* CHGDIR */

int chgdir(char *inp)
{
 char *path;
 char *pos;
 int ret, i;
 if(!strcmp(inp,".."))
 {
  path=(char *) malloc(strlen(dir)+1);
  strcpy(path,dir);
  i=strlen(path)-1;
  while(path[--i]!='/');
  path[++i]='\0';
  if(strlen(path)>=strlen(device)) 
   strcpy(dir,path);
  else return 1;
 }
 else
 {
  path=(char *) malloc(strlen(dir)+strlen(inp)+1);
  sprintf(path,"%s%s",dir,inp);
  ret=chkdir(path);
  if(!ret)
  {
   while(pos=strstr(path,"/../"))
   {
    i=0;
    while(*(--pos)!='/') i++;

    while(*(pos+i+4)!='\0')
    { 
     *pos=*(pos+i+4);
      pos++; 
    }
    *pos='\0';
   }
   while(pos=strstr(path,"./"))
   {
    while(*(pos+2)!='\0')
    {
     *pos=*(pos+2);
      pos++; 
    }
    *pos='\0';
   } 
   strcpy(dir,path);
   return 0;
  }
  else return 1;
 }
}

/* REMOVE DIR */

int removedir(char *dirname)
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
   if (entry->d_type == DT_DIR) removedir(path);
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

/* COPY FILE FROM USB */

int copyfilefromusb(char *path, int new_fd)
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
  send_msg(new_fd, sbuf);
  for(i=0;i<file_size;i++)
  {
   byte=getc(fp);
   if(feof(fp)) break;
   else
   {
    if(send(new_fd, &byte, 1, 0) == -1)
    { perror("Server-send()"); return 1; }
   }
  }
  type =lstat(path, &statbuff);
  if(type!=0)
   { perror("lstat()"); return 1; }
  else 
  { 
   strcpy(sbuf,itoa(statbuff.st_mode,10));
   send_msg(new_fd, sbuf);
  }
  return 0;
 }
 else return 1;
}

/* MOVE FILE FROM USB */

int movefilefromusb(char *path, int new_fd)
{
 int ret = copyfilefromusb(path,new_fd);
 unlink(path);
 return ret;
 
}

/* COPY FILE TO USB */

int copyfiletousb(char *file, char *dirname, int new_fd)
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
  strcpy(rbuf,recv_msg(new_fd));
  int file_size=atoi(rbuf);
  for(i=0;i<file_size;i++)
  { 
   if((numbytes = recv(new_fd, &ch, 1, 0)) == -1)
   { 
    perror("recv()");
    return(1);
   }
   else 
    putc(ch,fp); 
  }
  fclose(fp);
  strcpy(rbuf,recv_msg(new_fd));
  mode_t mode=atoi(rbuf);
  if(chmod(path, mode)) 
  {
   perror("fchmod()");
   return(1);
  }
 }
 return 0;
}

/* MOVE FILE TO USB */

int movefiletousb(char *file, char *dirname, int new_fd)
{
 int ret = copyfiletousb(file, dirname, new_fd);
 return ret;
}

/* COPY DIR FROM USB */

int copydirfromusb(char *path, int new_fd)
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
     send_msg(new_fd,"D");
     send_msg(new_fd,drd->d_name);
     sprintf(path1,"%s%s/",path,drd->d_name);
     ret=copydirfromusb(path1,new_fd);
     if(ret) return 1;
    }
    else if((statbuff.st_mode&S_IFMT)==S_IFREG)
    {
     send_msg(new_fd,"R"); 
     send_msg(new_fd, drd->d_name);
     sprintf(path1,"%s%s",path,drd->d_name);
     ret=copyfilefromusb(path1,new_fd);
     if(ret) return 1;
    }
   }
  }
 }
 send_msg(new_fd,"E");
 return 0;
}    

/* MOVE DIR FROM USB */

int movedirfromusb(char *path, int new_fd)
{
 int ret = copydirfromusb(path,new_fd);
 if (!ret )removedir(path);
 return ret;
}

/* COPY DIR TO USB */

int copydirtousb(char *path, int new_fd)
{
 int ret;
 char dirname[50], filename[50], path1[100];
 while(1)
 {
  strcpy(rbuf,recv_msg(new_fd));
  if(!strcmp("D",rbuf))
  {
   strcpy(dirname, recv_msg(new_fd));
   sprintf(path1,"%s%s/",path,dirname);
   if(makedir(path1)) return 1;
   ret=copydirtousb(path1, new_fd);
   if(ret) return 1;
  }
  else if(!strcmp("R",rbuf))
  {
   strcpy(filename, recv_msg(new_fd));
   ret=copyfiletousb(filename, path, new_fd);
   if(ret) return 1;
  }
  else if(!strcmp("E", rbuf)) return 0;  
 }
 return 0;
}

/* MOVE DIR TO USB */
 
int movedirtousb(char *path, int new_fd)
{
 int ret = copydirtousb(path,new_fd);
 return ret;
}

/* RENAME */

int renamedir( char* old, char * new)
{
 char *oldname=(char *) malloc(strlen(dir)+strlen(old)+1);
 char *newname=(char *) malloc(strlen(dir)+strlen(new)+1); 
 sprintf(oldname,"%s%s",dir,old);
 sprintf(newname,"%s%s",dir,new);  
 int ret = rename(oldname, newname);
 if(ret) perror("rename()");
 return ret;
}
