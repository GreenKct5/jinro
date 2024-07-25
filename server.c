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
    int playerNum = 4;
    int soc_waiting;
    char buf[BUF_LEN];

    printf("%s\n", hello_hachi());
    printf("%s\n", hello_koto());
    printf("%s\n", hello_noname());
    printf("%s\n", hello_takema());

    write(1,"このゲームは4人プレイです\n",strlen("このゲームは4人プレイです\n"));

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
    // Player型の配列を宣言
    Player players[playerNum];
    for (int i = 0; i < playerNum; i++) {
        players[i].sock = accept(soc_waiting, NULL, NULL);
        read(players[i].sock, players[i].name, BUF_LEN);
    }

    // 全員の参加を確認した後にタイマーを開始
    int client_socks[playerNum + 3];
    client_socks[0] = playerNum;
    client_socks[1] = minutes;
    client_socks[2] = seconds;
    

    pthread_t timer_thread;
    pthread_create(&timer_thread, NULL, timer, (void*)client_socks); // タイマーを別スレッドで起動

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
    // 投票
    // voting(playerNum, players);
    do{
        readset = readset_origin;
        select(players[playerNum-1].sock+1,&readset,NULL,NULL,NULL);
        for (int i = 0; i < playerNum; i++) {
            if(FD_ISSET(players[i].sock, &readset)) {
                int n = read(players[i].sock, buf, BUF_LEN);
                if (n > 0) {
                    buf[n] = '\0';

                    // 現在の時間を取得
                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    char time_str[256];
                    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

                    // 送信者の名前と時間を付加してメッセージを生成
                    char message[BUF_LEN + 256];
                    snprintf(message, sizeof(message), "(%s: %s) -> %s", players[i].name, time_str, buf);
                    for (int j = 0; j < playerNum; j++) {
                        if (j != i) {
                            write(players[j].sock, message, strlen(message));
                        }
                    }
                }
            }
        }
    }while(strncmp(buf,"quit",4) != 0);
    
    for(int i = 0;i < playerNum;i++) close(players[i].sock);
    pthread_join(timer_thread, NULL);
}