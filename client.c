#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include "./mylib/hachi.h"
#include "./mylib/koto.h"
#include "./mylib/noname.h"
#include "./mylib/takema.h"

#define PORT                (in_port_t)50000
#define BUF_LEN             512

int main()
{
    struct sockaddr_in server;
    int soc;
    char ip_str[BUF_LEN];
    struct in_addr ip_addr;
    char buf[BUF_LEN];
    char username[BUF_LEN];

    /*
    input server's ip 
    */
    write(1,"Please input server's address : ",strlen("Please input server's address : "));
    read(0,ip_str,BUF_LEN);

    write(1,"Please input your name        : ",strlen("Please input your name        : "));
    read(0,username,BUF_LEN);
    chop_newline(username,BUF_LEN);

    inet_aton(ip_str,&ip_addr);                 // address(text) to network address(binary)
    memset((char *)&server,0,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memcpy((char *)&server.sin_addr,&ip_addr,sizeof(ip_addr));

    if((soc = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    
    if(connect(soc,(struct sockaddr *)&server,sizeof(server)) == -1){
        perror("connect");
        exit(1);
    }
    write(1,"Go Ahead!\n",strlen("Go Ahead!"));
    // await-async chat 
    fd_set readset,readset_origin;
    int fd = soc;
    
    FD_ZERO(&readset);

    FD_SET(0,&readset);
    FD_SET(fd,&readset);
    readset_origin = readset;

    do{
    readset = readset_origin;
    select(fd+1,&readset,NULL,NULL,NULL);
        if(FD_ISSET(0,&readset)){
            time_t now;
            struct tm *t;
            time(&now);
            t = localtime(&now);
            char time[256];
            int timeLen = strftime(time,256,", %X)-> ",t);

            int n = read(0,buf,BUF_LEN);

            write(fd,"(",strlen("("));
            write(fd,username,strlen(username));
            write(fd,time,timeLen);
            write(fd,buf,n);
        }
        if(FD_ISSET(fd,&readset)){
            int n;
            n = read(fd,buf,BUF_LEN);
            write(1,buf,n);
        }
    }while(strncmp(buf,"quit",4) != 0);
    
    close(soc);
}