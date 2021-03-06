#include "../inc/usbshare_client.h"
  
main(int argc, char *argv[])
{
 int numbytes, i, ret;
 char ch, file[MAXDATASIZE], dirname1[MAXDATASIZE], dirname2[MAXDATASIZE], path[MAXDATASIZE];
 if(argc != 2)
 {
  fprintf(stderr, "Usage: %s <server_hostname or IP-addr>\n", argv[0]);
  exit(1);
 }
 strcpy(host, argv[1]);
 sockfd=con_to_serv(host);

 /* GET DEV LST */
 
 strcpy(rbuf,recv_msg(sockfd));
 printf("\nAvailable USB Storage Devices in server:\n\n");
 printf("%s", rbuf);
 strcpy(rbuf,recv_msg(sockfd));
 printf("\nEnter the serial number of device to connect to (<=%d): ",atoi(rbuf));
 scanf("%s",sbuf);
 printf("\n");
 if(atoi(sbuf)<1||atoi(sbuf)>atoi(rbuf))
 { 
  printf("Invalid input!\n\n"); 
  exit(1);
 }
 else
 send_msg(sockfd,sbuf);
 strcpy(device,recv_msg(sockfd));
 strcpy(dir, device);

 while(1)
 {
  printf("> %s > %s > $ ", host, dir);
  scanf("%s",sbuf);

  /* HELP */

  if(!strcmp(sbuf,"help"))
  {
   printf("\nCOMMANDS:\n\nlist\n\nfreespace\n\nmakedir <directory name>\n\nrmv <directory name or file name>\n\nchdir <directory name>\n\nrename \"<old dir|file name>\" \"<new dir|file name>\"\n\ncpfile \"s:<relative path to file in USB>\" \"c:<absolute path in client>\"\n\ncpfile \"c:<absolute path to file in client>\" [\"s:<relative path in USB>\"]\n\nmvfile \"s:<relative path to file in USB>\" \"c:<absolute path in client>\"\n\nmvfile \"c:<absolute path to file in client>\" [\"s:<relative path in USB>\"]\n\ncpdir \"s:<relative path to dir in USB>\" \"c:<absolute path in client>\"\n\ncpdir \"c:<absolute path to dir in client>\" [\"s:<relative path in USB>\"]\n\nmvdir \"s:<relative path to dir in USB>\" \"c:<absolute path in client>\"\n\nmvdir \"c:<absolute path to directory in client>\" [\"s:<relative path in USB>\"]\n\nquit\n\nhelp\n\n");
  }

  /* LIST */
  
  else if(!strcmp(sbuf,"list"))
  {
   send_msg(sockfd, sbuf);
   i=0;
   if((ch=getchar())=='\n') send_msg(sockfd,".");
   else if(ch==' ')
   {
    while(ch=getchar())
    {
     if(ch=='\n') break;
     else sbuf[i++]=ch;
    }
    if(sbuf[i-1]!='/') sbuf[i++]='/';
    sbuf[i]='\0';
    send_msg(sockfd,sbuf);
   }
   strcpy(rbuf,recv_msg(sockfd));
   printf("%s", rbuf);
  }

  /* FREESPACE */

  else if(!strcmp(sbuf,"freespace"))
  {
   send_msg(sockfd,sbuf);
   strcpy(rbuf,recv_msg(sockfd));
   printf("%s", rbuf);
  } 

  /* MAKEDIR */

  else if(!strcmp(sbuf,"makedir"))
  {
   send_msg(sockfd,sbuf);
   i=0;
   ch=getchar();
   while(ch=getchar())
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   sbuf[i]='\0';
   send_msg(sockfd, sbuf);
   strcpy(rbuf,recv_msg(sockfd));
   printf("%s\n", rbuf);
  }

  /* RMV */

  else if(!strcmp(sbuf,"rmv"))
  { 
   send_msg(sockfd,sbuf);
   i=0;
   ch=getchar();
   while(ch=getchar())
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(sbuf[i-1]=='/') sbuf[i-1]='\0';
   else sbuf[i]='\0';
   send_msg(sockfd, sbuf);
   fflush(stdin);
   strcpy(rbuf,recv_msg(sockfd));
   if(!strcmp(rbuf,"confirm"))
   {
    printf("Directory is not empty. Do you want to continue (y/n) : ");
    scanf("%s",sbuf);
    if(!strcmp(sbuf,"y") || !strcmp(sbuf,"Y")) send_msg(sockfd, "y");
    else send_msg(sockfd, "n");
    strcpy(rbuf,recv_msg(sockfd));
   }
   printf("%s\n", rbuf);
  }

  /* CHDIR */
  
  else if(!strcmp(sbuf,"chdir"))
  { 
   send_msg(sockfd,sbuf);
   i=0;
   ch=getchar();
   while(ch=getchar())
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(sbuf[i-1]!='/' && sbuf[i-1]!='.') sbuf[i++]='/';
   sbuf[i]='\0';
   send_msg(sockfd, sbuf);
   strcpy(rbuf,recv_msg(sockfd));
   if(!strcmp(rbuf,"Invalid operation!"))
   printf("%s\n",rbuf);
   else
   strcpy(dir, rbuf);
  }

  /* CPFILE */

  else if(!strcmp(sbuf,"cpfile"))
  { 
   i=0;
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   if(sbuf[i-1]=='/') sbuf[i-1]='\0';
   else sbuf[i]='\0';
   if(sbuf[0]=='s' && sbuf[1]==':')
   {
    strcpy(file,sbuf+2);
    while((ch=getchar())!='"'&& ch!='\n');
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    i=0;
    while((ch=getchar())!='"')
    {
     if(ch=='\n') break;
     else dirname1[i++]=ch;
    }
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    if(dirname1[i-1]!='/') dirname1[i++]='/';
    dirname1[i]='\0';
    if(dirname1[0]=='c' && dirname1[1]==':')
    {
     send_msg(sockfd,"cpfilefrom");
     send_msg(sockfd, file);
     i=strlen(file);
     while(file[i--]!='/' && i>=0);
     if(file[++i]=='/') strcpy(file,&file[++i]);   
     ret=cpfilefrom(file, dirname1+2);
    }
    else printf("Invalid input!\n");
   }
   else if(sbuf[0]=='c' && sbuf[1]==':')
   {
    while((ch=getchar())!='"' && ch!='\n');
    if(ch=='\n') strcpy(dirname2,".");
    else
    {
     i=0;
     while((ch=getchar())!='"')
     {
      if(ch=='\n') break;
      else dirname2[i++]=ch;
     }
     if(dirname2[i-1]!='/') dirname2[i++]='/';
     dirname2[i]='\0';
     
     if(dirname2[0]=='s' && dirname2[1]==':')
      strcpy(dirname2, dirname2+2);
     else printf("Invalid input!\n");
    }
    strcpy(sbuf,sbuf+2);
    strcpy(path,sbuf);
    i=strlen(sbuf);
    while(sbuf[--i]!='/');
    strcpy(file,&sbuf[++i]);
    send_msg(sockfd,"cpfileto");
    send_msg(sockfd, file);
    send_msg(sockfd, dirname2);
    ret=cpfileto(path);    
   }
   else printf("Invalid input!\n");
   if(ret) printf("Invalid input!\n");
   else
   {
    strcpy(rbuf,recv_msg(sockfd));
    printf("%s\n", rbuf);
   }
  }

  /* MVFILE */

  else if(!strcmp(sbuf,"mvfile"))
  { 
   i=0;
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   if(sbuf[i-1]=='/') sbuf[i-1]='\0';
   else sbuf[i]='\0';
   if(sbuf[0]=='s' && sbuf[1]==':')
   {
    strcpy(file,sbuf+2);
    while((ch=getchar())!='"'&& ch!='\n');
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    i=0;
    while((ch=getchar())!='"')
    {
     if(ch=='\n') break;
     else dirname1[i++]=ch;
    }
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    if(dirname1[i-1]!='/') dirname1[i++]='/';
    dirname1[i]='\0';
    if(dirname1[0]=='c' && dirname1[1]==':')
    {
     send_msg(sockfd,"mvfilefrom");
     send_msg(sockfd, file);
     i=strlen(file);
     while(file[i--]!='/' && i>=0);
     if(file[++i]=='/') strcpy(file,&file[++i]);   

     ret=mvfilefrom(file, dirname1+2);
    }
    else printf("Invalid input!\n");
   }
   else if(sbuf[0]=='c' && sbuf[1]==':')
   {
    while((ch=getchar())!='"' && ch!='\n');
    if(ch=='\n') strcpy(dirname2,".");
    else
    {
     i=0;
     while((ch=getchar())!='"')
     {
      if(ch=='\n') break;
      else dirname2[i++]=ch;
     }
     if(dirname2[i-1]!='/') dirname2[i++]='/';
     dirname2[i]='\0';
     
     if(dirname2[0]=='s' && dirname2[1]==':')
      strcpy(dirname2, dirname2+2);
     else printf("Invalid input!\n");
    }
    strcpy(sbuf,sbuf+2);
    strcpy(path,sbuf);
    i=strlen(sbuf);
    while(sbuf[--i]!='/');
    strcpy(file,&sbuf[++i]);
    send_msg(sockfd,"mvfileto");
    send_msg(sockfd, file);
    send_msg(sockfd, dirname2);
    ret=mvfileto(path);    
   }
   else printf("Invalid input!\n");
   if(ret) printf("Invalid input!\n");
   else
   {
    strcpy(rbuf,recv_msg(sockfd));
    printf("%s\n", rbuf);
   }
  }

  /* CPDIR */

  else if(!strcmp(sbuf,"cpdir"))
  { 
   i=0;
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   if(sbuf[i-1]!='/') sbuf[i++]='/';
   sbuf[i]='\0';
   if(sbuf[0]=='s' && sbuf[1]==':')
   {
    strcpy(dirname1,sbuf);
    while((ch=getchar())!='"'&& ch!='\n');
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    i=0;
    while((ch=getchar())!='"')
    {
     if(ch=='\n') break;
     else dirname2[i++]=ch;
    }
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    if(dirname2[i-1]!='/') dirname2[i++]='/';
    dirname2[i]='\0'; 
    if(dirname2[0]=='c' && dirname2[1]==':')
    {
     send_msg(sockfd,"cpdirfrom"); 
     send_msg(sockfd, dirname1+2);
     i=strlen(dirname1)-2;
     while(dirname1[i--]!='/' && i>=0);
     if(dirname1[++i]=='/') strcpy(dirname1,&dirname1[++i]);  
     else strcpy(dirname1, dirname1+2);
     char *path=(char *) malloc(strlen(dirname2+2)+strlen(dirname1)+1);
     sprintf(path,"%s%s",dirname2+2,dirname1);
     if(makedir(path)) return 1;
     ret=cpdirfrom(path);
    }
    else printf("Invalid Input!");
   }
   else if(sbuf[0]=='c' && sbuf[1]==':')
   {
    while((ch=getchar())!='"' && ch!='\n');
    if(ch=='\n') strcpy(dirname2,".");
    else
    {
     i=0;
     while((ch=getchar())!='"')
     {
      if(ch=='\n') break;
      else dirname2[i++]=ch;
     }
     if(dirname2[i-1]!='/') dirname2[i++]='/';
     dirname2[i]='\0';

     if(dirname2[0]=='s' && dirname2[1]==':')
      strcpy(dirname2,dirname2+2);
     else printf("Invalid input!\n"); 
    }
    strcpy(sbuf,sbuf+2);
    strcpy(path,sbuf);
    i=strlen(sbuf)-1;
    while(sbuf[--i]!='/');
    strcpy(file,&sbuf[++i]);
    send_msg(sockfd,"cpdirto");
    send_msg(sockfd, file);
    send_msg(sockfd, dirname2);
    ret=cpdirto(path); 
   }
   else printf("Invalid input!\n");
   if(ret) printf("Invalid input!\n");
   else
   {
    strcpy(rbuf,recv_msg(sockfd));
    printf("%s\n", rbuf);
   }
  }

  /* MVDIR */

  else if(!strcmp(sbuf,"mvdir"))
  { 
   i=0;
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   if(sbuf[i-1]!='/') sbuf[i++]='/';
   sbuf[i]='\0';
   if(sbuf[0]=='s' && sbuf[1]==':')
   {
    strcpy(dirname1,sbuf);
    while((ch=getchar())!='"'&& ch!='\n');
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    i=0;
    while((ch=getchar())!='"')
    {
     if(ch=='\n') break;
     else dirname2[i++]=ch;
    }
    if(ch=='\n') 
    {
     printf("Invalid input!\n");
     continue;
    }
    if(dirname2[i-1]!='/') dirname2[i++]='/';
    dirname2[i]='\0';
    if(dirname2[0]=='c' && dirname2[1]==':')
    {
     send_msg(sockfd,"mvdirfrom");    
     send_msg(sockfd, dirname1+2);
     i=strlen(dirname1)-2;
     while(dirname1[i--]!='/' && i>=0);
     if(dirname1[++i]=='/') strcpy(dirname1,&dirname1[++i]); 
     else strcpy(dirname1,dirname1+2); 
     char *path=(char *) malloc(strlen(dirname2+2)+strlen(dirname1)+1);
     sprintf(path,"%s%s",dirname2+2,dirname1);
     if(makedir(path)) return 1;
     ret=mvdirfrom(path);
    }
    else printf("Invalid Input!");
   }
   else if(sbuf[0]=='c' && sbuf[1]==':')
   {
    while((ch=getchar())!='"' && ch!='\n');
    if(ch=='\n') strcpy(dirname2,".");
    else
    {
     i=0;
     while((ch=getchar())!='"')
     {
      if(ch=='\n') break;
      else dirname2[i++]=ch;
     }
     if(dirname2[i-1]!='/') dirname2[i++]='/';
     dirname2[i]='\0';

     if(dirname2[0]=='s' && dirname2[1]==':')
      strcpy(dirname2,dirname2+2);
     else printf("Invalid input!\n"); 
    }
    strcpy(sbuf,sbuf+2);
    strcpy(path,sbuf);
    i=strlen(sbuf)-1;
    while(sbuf[--i]!='/');
    strcpy(file,&sbuf[++i]);
    send_msg(sockfd,"mvdirto");
    send_msg(sockfd, file);
    send_msg(sockfd, dirname2);
    ret=mvdirto(path); 
   }
   else printf("Invalid input!\n");
   if(ret) printf("Invalid input!\n");
   else
   {
    strcpy(rbuf,recv_msg(sockfd));
    printf("%s\n", rbuf);
   }
  }

  /* RENAME */

  else if(!strcmp(sbuf,"rename"))
  { 
   send_msg(sockfd, "rename");
   char oldname[MAXDATASIZE], newname[MAXDATASIZE];
   i=0;
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else sbuf[i++]=ch;
   }
   sbuf[i]='\0';  
   strcpy(oldname,sbuf);
   send_msg(sockfd, oldname);
   while((ch=getchar())!='"' && ch!='\n');
   if(ch=='\n') 
   {
    printf("Invalid input!\n");
    continue;
   }     
   i=0;
   while((ch=getchar())!='"')
   {
    if(ch=='\n') break;
    else dirname1[i++]=ch;
   }
   dirname1[i]='\0';
   strcpy(newname,dirname1);
   send_msg(sockfd,newname );
   strcpy(rbuf,recv_msg(sockfd));
   printf("%s\n", rbuf);
  }
   
  /* QUIT */ 
 
  else if(!strcmp(sbuf,"quit"))
  {
   send_msg(sockfd, "quit");
   exit(0);
  }
  
  /* COMMAND NOT FOUND */

  else
  {
   while((ch=getchar())!='\n');
   printf("Command not found!\n");
  }   
  fflush(stdout);
 }
 close(sockfd);
 return 0;
}
