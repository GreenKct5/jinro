#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<time.h>

#define PORT    (in_port_t)50000
#define BUF_LEN 512

//#server
void main(){
    struct sockaddr_in me;
    int soc_waiting;
    int soc;
    char buf[BUF_LEN];
    fd_set readset;
    int fd;

    char myname[64] = "\0";
    char villagersA[64] = "\0";
    char villagersB[64] = "\0";
    char villagersC[64] = "\0";
    char chatroom[1000] = "\0";

    write(1,"Input your name:", 16);
    read(0, myname, 100);

    FD_ZERO(&readset);
    FD_SET(0,&readset);
    FD_SET(soc,&readset);

    myname[strlen(myname)-1] = '\0';

    send(soc,myname,64,0);
    recv(soc,villagersA,64,0);
    recv(soc,villagersB,64,0);
    recv(soc,villagersC,64,0);

    snprintf(chatroom,512,"今回の村人は，%S，%S，%S，%Sです", myname, villagersA, villagersB, villagersC);
    write(1,chatroom,512);
    
}

//#client
void main(){
    struct sockaddr_in server;
    int soc;
    char ip_str[] = "server_address"
    struct in_addr ip_addr;
    char buf[BUF_LEN];
    fd_set readset;
    int fd;

    char myname[64] = "\0";
    char villagersA[64] = "\0";
    char villagersB[64] = "\0";
    char villagersC[64] = "\0";
    char chatroom[1000] = "\0";

    write(1,"Input your name:", 16);
    read(0, myname, 100);

    FD_ZERO(&readset);
    FD_SET(0,&readset);
    FD_SET(soc,&readset);

    myname[strlen(myname)-1] = '\0';

    send(soc,myname,64,0);
    recv(soc,villagersA,64,0);
    recv(soc,villagersB,64,0);
    recv(soc,villagersC,64,0);

    snprintf(chatroom,sizeof(chatroom),"今回の村人は，%S，%S，%S，%Sです", myname, villagersA, villagersB, villagersC);
    write(1,chatroom,sizeof(chatroom));
}