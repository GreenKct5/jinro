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
#include "./role.h"

#define BUF_LEN 512

int VoteDisclosure(int *votes, int playerNum);
int getWolfNum(Player * players, int playerNum);

// 配列から改行ををなくす
char * chop_newline(char *str,int len){
    int n = strlen(str);
    if(n<len && str[n-1]=='\n'){
        str[n-1] = '\0';
    }
    return str;
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

void divination(Player* seer, Player* players, int playerNum) {
    char buf[BUF_LEN];
    char option[64];
    int optionIndex = 0;
    int choice;

    // 占い師に占う選択肢を表示
    write(seer->sock, "誰を占いますか\n", strlen("誰を占いますか\n"));

    for (int i = 0; i < playerNum; i++) {
        if (strcmp(players[i].name, seer->name) != 0) {
            snprintf(option, sizeof(option), "%c. %s\n", 'a' + optionIndex, players[i].name);
            write(seer->sock, option, strlen(option));
            optionIndex++;
        }
    }

    snprintf(option, sizeof(option), "%c. 使われてない役職2つをみる\n", 'a' + optionIndex);
    write(seer->sock, option, strlen(option));
    write(seer->sock, "入力: ", strlen("入力: "));

    // 占い師からの入力を読み取る
    read(seer->sock, buf, BUF_LEN);
    chop_newline(buf, BUF_LEN);

    // 入力の処理
    choice = buf[0] - 'a';
    if (choice >= 0 && choice < optionIndex) {
        // 占う相手の役職を表示
        write(seer->sock, "占った結果:\n", strlen("占った結果:\n"));
        snprintf(buf, BUF_LEN, "%sさんの役職は%sです。\n", players[choice].name, strRole(players[choice].role));
        write(seer->sock, buf, strlen(buf));
    } else if (choice == optionIndex) {
        // 使われてない役職2つを表示
        write(seer->sock, "使われていない役職2つは:\n", strlen("使われていない役職2つは:\n"));

        // 全ての役職とその数をカウント
        Role allRoles[8] = {WEREWOLF, WEREWOLF, SEER, THIEF, VILLAGER, VILLAGER, VILLAGER, VILLAGER};
        int roleCounts[4] = {0}; // 役職のカウント
        
        // プレイヤーの役職をカウント
        for (int i = 0; i < playerNum; i++) {
            roleCounts[players[i].role]++;
        }

        // 使われていない役職を表示
        for (int i = 0; i < 8; i++) {
            if (roleCounts[allRoles[i]] == 0) {
                snprintf(buf, BUF_LEN, "%s\n", strRole(allRoles[i]));
                write(seer->sock, buf, strlen(buf));
            }
        }
    } else {
        // 無効な入力の場合の処理
        write(seer->sock, "無効な選択です\n", strlen("無効な選択です\n"));
    }
}

int stealRole(Player *thief, Player *victim) {
    if (thief->role != THIEF || victim->role == THIEF) return -1;

    thief->role = victim->role;
    victim->role = VILLAGER;

    return 0;
}

int selectVictim(Player *thief, Player *players, int playerNum) {
    char buf[BUF_LEN];
    int victimNum = 0;

    while (1) {
        snprintf(buf, BUF_LEN, "誰から役職を盗みますか？\n");
        write(thief->sock, buf, strlen(buf));
        for (int i = 0; i < playerNum; i++) {
            snprintf(buf, BUF_LEN, "%d: %s\n", i + 1, players[i].name);
            write(thief->sock, buf, strlen(buf));
        }
        write(thief->sock, "番号を入力: ", strlen("番号を入力: "));
        read(thief->sock, buf, BUF_LEN);
        sscanf(buf, "%d", &victimNum);
        victimNum--;

        if (victimNum < 0 || victimNum >= playerNum) {
            snprintf(buf, BUF_LEN, "そのプレイヤーは存在しません\n");
            write(thief->sock, buf, strlen(buf));
            continue;
        }
        break;
    }

    int choice = 0;
    while (1) {
        snprintf(buf, BUF_LEN, "%sさんの役職は%sです。\n役職を盗みますか？\n1: 盗む\n2: 盗まない\n", players[victimNum].name, strRole(players[victimNum].role));
        write(thief->sock, buf, strlen(buf));
        read(thief->sock, buf, BUF_LEN);
        sscanf(buf, "%d", &choice);

        switch (choice) {
            case 1:
                stealRole(thief, &players[victimNum]);
                snprintf(buf, BUF_LEN, "%sさんの役職を盗みました\nあなたの役職は%sです。\n", players[victimNum].name, strRole(thief->role));
                write(thief->sock, buf, strlen(buf));
                break;
            case 2:
                snprintf(buf, BUF_LEN, "盗みませんでした。\nあなたは市民陣営です。\n");
                write(thief->sock, buf, strlen(buf));
                break;
            default:
                snprintf(buf, BUF_LEN, "1か2を入力してください\n");
                write(thief->sock, buf, strlen(buf));
                continue;
        }
        break;
    }
    return 0;
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
        write(sock, "投票結果: ", strlen("投票結果: "));
        if (maxIndex != -1) {
            snprintf(option, sizeof(option), "%s が一番多く投票されました.", players[maxIndex].name);
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
    // Player wolfList[2] = {0}; // 人狼プレイヤーの配列
    int wolfNum = 0;
    for(int i=0; i<playerNum; i++){
            if(players[i].role == WEREWOLF){
                // wolfList[wolfNum] = players[i];
                wolfNum++;
            }
        }
    return wolfNum;
}