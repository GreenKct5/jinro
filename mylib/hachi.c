#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "./takema.h"

char * hello_hachi(){
    return "Hello. I'm hachi!";
}

void displayPlayersName(int playerNum, Player *players){
    const char *header = "プレイヤーの一覧\n";

    for(int i = 0; i < playerNum; i++){
        int sock = players[i].sock;
        write(sock, "\n", strlen("\n"));
        write(sock, header, strlen(header));
        for(int j = 0; j < playerNum; j++){
            write(sock, players[j].name, strlen(players[j].name));
            write(sock, "\n", strlen("\n"));
        }
    }
}
