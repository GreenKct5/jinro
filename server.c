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
#include <pthread.h>
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

    // タイマーの分と秒を設定
    int minutes = 2;
    int seconds = 30;
    // write(1,"Please input timer minutes :",strlen("Please input timer minutes :"));
    // scanf("%d", &minutes);
    // write(1,"Please input timer seconds :",strlen("Please input timer seconds :"));
    // scanf("%d", &seconds);
    
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
    int soc_comm[playerNum];
    for ( int i = 0; i < playerNum; i++) soc_comm[i] = accept(soc_waiting,NULL,NULL);
    
    close(soc_waiting);

    // 全員の参加を確認した後にタイマーを開始
    int client_socks[playerNum + 3];
    client_socks[0] = playerNum;
    client_socks[1] = minutes;
    client_socks[2] = seconds;
    for (int i = 0; i < playerNum; i++) {
        client_socks[i + 3] = soc_comm[i];
    }
    pthread_t timer_thread;
    pthread_create(&timer_thread, NULL, timer, (void*)client_socks); // タイマーを別スレッドで起動

    // await-async chat 
    fd_set readset,readset_origin;
    FD_ZERO(&readset);
    for(int i = 0; i < playerNum; i++) FD_SET(soc_comm[i],&readset);
    readset_origin = readset;

    do{
        readset = readset_origin;
        select(soc_comm[playerNum-1]+1,&readset,NULL,NULL,NULL);
        for (int i = 0; i < playerNum; i++){
            if(FD_ISSET(soc_comm[i],&readset)){
                int n = read(soc_comm[i],buf,BUF_LEN);
                for (int j = 1;j < playerNum;j++) write(soc_comm[(i+j)%playerNum],buf,n);
                write(1,buf,n);
            }
        }
    }while(strncmp(buf,"quit",4) != 0);
    
    for(int i = 0;i < playerNum;i++) close(soc_comm[i]);
    pthread_join(timer_thread, NULL);
}