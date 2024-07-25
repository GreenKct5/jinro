#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include "./mylib/hachi.h"
#include "./mylib/koto.h"
#include "./mylib/noname.h"
#include "./mylib/takema.h"

#define PORT (in_port_t)    50000
#define BUF_LEN             512

int main()
{
    struct sockaddr_in me;
    int playerNum;
    int soc_waiting;
    char buf[BUF_LEN];
    char username[BUF_LEN];
    char message[] = "Please input your name        : "; 

    printf("%s\n", hello_hachi());
    printf("%s\n", hello_koto());
    printf("%s\n", hello_noname());
    printf("%s\n", hello_takema());

    write(1,message,strlen(message));
    read(0,username,BUF_LEN);
    chop_newline(username,BUF_LEN);
    write(1,"Please input player num    :",strlen("Please input player num    :"));
    scanf("%d",&playerNum);
    
    memset((char *)&me,0,sizeof(me));           // initialize "me"
    me.sin_family = AF_INET;                    // configure protocol (AF_INET: IPv4)
    me.sin_addr.s_addr = htonl(INADDR_ANY);     // configure own IP Address (INADDR_ANY: 0.0.0.0)
    me.sin_port = htons(PORT);                  // configure port number

    if((soc_waiting = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    
    if(bind(soc_waiting,(struct sockaddr *)&me,sizeof(me)) == -1){
        perror("bind");
        exit(1);
    }

    listen(soc_waiting,playerNum);
    // Player型の配列を宣言
    Player players[playerNum];
    for ( int i = 0; i < playerNum; i++) players[i].sock = accept(soc_waiting,NULL,NULL);

    Role trash[2]; // 使わない役職を格納する配列 
    randomRole(players,trash,playerNum); // 役職をランダムに割り振る

    // 割り振った役職をクライアントに表示
    for (int i = 0; i < playerNum; i++) {
        snprintf(buf, BUF_LEN, "\nあなたの役職は %s です\n", strRole(players[i].role));
        write(players[i].sock, buf, strlen(buf));
    }
    close(soc_waiting);
    
    // await-async chat 
    fd_set readset,readset_origin;
    FD_ZERO(&readset);
    for(int i = 0; i < playerNum; i++) FD_SET(players[i].sock,&readset);
    readset_origin = readset;
    do{
        readset = readset_origin;
        select(players[playerNum-1].sock+1,&readset,NULL,NULL,NULL);
        for (int i = 0; i < playerNum; i++){
            if(FD_ISSET(players[i].sock,&readset)){
                int n = read(players[i].sock,buf,BUF_LEN);
                for (int j = 1;j < playerNum;j++) write(players[(i+j)%playerNum].sock,buf,n);
                write(1,buf,n);
            }
        }
    }while(strncmp(buf,"quit",4) != 0);
    
    for(int i = 0;i < playerNum;i++) close(players[i].sock);
}

