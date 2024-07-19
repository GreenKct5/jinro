#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include "./takema.h"

#define BUF_LEN 512

int VoteDisclosure(int *votes, int playerNum);
int getWolfNum(Player * players, int playerNum);

char * hello_koto() {
    return "Hello. I'm koto!";
}

// 配列から改行ををなくす
char * chop_newline(char *str,int len){
    int n = strlen(str);
    if(n<len && str[n-1]=='\n'){
        str[n-1] = '\0';
    }
    return str;
}

// 投票
void voting(int playerNum, Player *players) {
    char buf[BUF_LEN];
    int votes[playerNum];
    int wolfNum = getWolfNum(players, playerNum);
    memset(votes, 0, sizeof(votes));

    for (int i = 0; i < playerNum; i++) {
        int sock = players[i].sock;
        char option[64];
        int optionIndex = 1;

        write(sock, "誰に投票しますか\n", strlen("誰に投票しますか\n"));

        // 選択肢を表示する
        for (int j = 0; j < playerNum; j++) {
            if (strcmp(players[i].name, players[j].name) != 0) {
                snprintf(option, sizeof(option), "%d. %s\n", optionIndex, players[j].name);
                write(sock, option, strlen(option));
                optionIndex++;
            }
        }
        snprintf(option, sizeof(option), "%d. この中に人狼はいない\n", optionIndex);
        write(sock, option, strlen(option));

        // 個人での投票
        write(sock, "入力: ", strlen("入力: "));
        read(sock, buf, BUF_LEN);
        chop_newline(buf, BUF_LEN);

        if ((int)buf[0] >= (int)'1' && (int)buf[0] <= (int)'4') {
            votes[buf[0] - 'a']++;
        } else {
            write(sock, "無効な票です\n", strlen("無効な票です\n"));
        }
    }

    int maxIndex = VoteDisclosure(votes, playerNum);
    for(int i = 0; i < playerNum; i++){
        int sock = players[i].sock;
        char option[64];
        write(sock, "投票結果:\n", strlen("投票結果:\n"));
        if (maxIndex != -1) {
            snprintf(option, sizeof(option), "%s が一番多く投票されました", players[maxIndex].name);
            write(sock, option, strlen(option));
            if(players[maxIndex].role == WEREWOLF) write(sock, "村人陣営の勝ちです\n", strlen("村人陣営の勝ちです\n"));
            else if(players[maxIndex].role != WEREWOLF && wolfNum == 0) write(sock, "人狼はいませんでした．負けです\n", strlen("人狼はいませんでした．負けです\n"));
            else write(sock, "人狼陣営の勝ちです\n", strlen("人狼陣営の勝ちです\n"));
        
        } else {
            write(sock, "処刑者はいませんでした\n", strlen("処刑者はいませんでした\n"));
            if(wolfNum == 0) write(sock, "人狼はいませんでした．村人陣営の勝ちです\n", strlen("人狼はいませんでした．村人陣営の勝ちです\n"));
            else write(sock, "この中に人狼がいました．人狼陣営の勝ちです\n", strlen("この中に人狼がいました．人狼陣営の勝ちです\n"));
        }
    }
}

// 票の開示とどの票が一番多かったか集計する
int VoteDisclosure(int *votes, int playerNum){
    int maxVotes = 0;
    int maxIndex = -1;
    for (int i = 0; i < playerNum; i++) {
        if (votes[i] > maxVotes) {
            maxVotes = votes[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

// 人狼の人数を数える
int getWolfNum(Player * players, int playerNum){
    Player wolfList[2] = {0}; // 人狼プレイヤーの配列
    int wolfNum = 0;
    for(int i=0; i<playerNum; i++){
            if(players[i].role == WEREWOLF){
                wolfList[wolfNum] = players[i];
                wolfNum++;
            }
        }
    return wolfNum;
}