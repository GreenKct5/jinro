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
#include "takema.h"
#define MAX_NAME_LENGTH 15
#define BUF_LEN 256


char * hello_takema(){
    return "Hello. I'm takema!";
}

char * strRole(Role role){
    switch (role)
    {
    case VILLAGER:
        return "村人";
        break;
    case WEREWOLF:
        return "人狼";
        break;
    case SEER:
        return "占い師";
        break;
    case THIEF:
        return "怪盗";
        break;
    default:
        return "";
        break;
    }
}

/// @brief プレイヤーにランダムに役職を振り分ける関数
/// @param players Player型の配列
/// @param trash 使わない役職のRole型の配列[2]
/// @param playerNum プレイヤー人数(max:6人)
int randomRole(Player *players,Role* trash,int playerNum){
    Role roleCards[8] = {WEREWOLF,WEREWOLF,SEER,THIEF,VILLAGER,VILLAGER,VILLAGER,VILLAGER};
    int useCardsNum = playerNum + 2;
    srand((unsigned int)time(NULL));
    // 役割をシャッフル
    for (int i = 0; i < useCardsNum;i++) {
        int j = rand() % useCardsNum;
        Role temp = roleCards[i];
        roleCards[i] = roleCards[j];
        roleCards[j] = temp;
    }

    // シャッフルした役割をプレイヤーに登録
    for (int i = 0; i < playerNum; i++) {
        if (i < playerNum) players[i].role = roleCards[i];
    }
    for (int i = 0; i < 2;i++) trash[i] = roleCards[playerNum + i];
    return 0;
}
/// @brief 怪盗と関数
/// @param thief 怪盗のプレイヤーのアドレス
/// @param victim 役割を盗まれるプレイヤーのアドレス
int stealRole(Player* thief,Player* victim){
    if (thief->role != THIEF||victim->role == THIEF) return -1; // 怪盗でない、または盗まれる役職が怪盗の場合はエラー

    // 役職を盗み、被害者の役職を村人にする
    thief->role = victim->role;
    victim->role = VILLAGER;

    return 0;
}
/// @brief 怪盗が盗む役職を選択する関数
/// @param thief 怪盗のプレイヤーのアドレス
/// @param players プレイヤーの配列
/// @param playerNum プレイヤー人数
int selectVictim(Player* thief,Player* players,int playerNum){
    char buf[BUF_LEN];
    int victimNum = 0;

    while(1){
        snprintf(buf,BUF_LEN,"誰から役職を盗みますか？\n");
        write(thief->sock,buf,strlen(buf));
        for (int i = 0; i < playerNum; i++) {
            snprintf(buf,BUF_LEN,"%d: %s\n",i+1,players[i].name);
            write(thief->sock,buf,strlen(buf));
        }
        read(thief->sock,buf,BUF_LEN);
        sscanf(buf,"%d",&victimNum);
        victimNum--;

        if (victimNum < 0 || victimNum >= playerNum) {
            snprintf(buf,BUF_LEN,"そのプレイヤーは存在しません\n");
            write(thief->sock,buf,strlen(buf));
            continue;
        }
        break;
    }
    int choice = 0;
    while(1){
        snprintf(buf,BUF_LEN,"%sさんの役職は%sです。\n役職を盗みますか？\n1: 盗む\n2: 盗まない\n",players[victimNum].name,strRole(players[victimNum].role));
        write(thief->sock,buf,strlen(buf));
        read(thief->sock,buf,BUF_LEN);
        sscanf(buf,"%d",&choice);
        switch (choice)
        {
        case 1:
            stealRole(thief,&players[victimNum]);
            snprintf(buf,BUF_LEN,"%sさんの役職を盗みました\nあなたの役職は%sです。",players[victimNum].name,strRole(thief->role));
            write(thief->sock,buf,strlen(buf));
            break;
        case 2:
            snprintf(buf,BUF_LEN,"盗みませんでした。\nあなたは市民陣営です。\n");
            write(thief->sock,buf,strlen(buf));
            break;
        default:
            snprintf(buf,BUF_LEN,"1か2を入力してください\n");
            write(thief->sock,buf,strlen(buf));
            continue;
        }
    }
    return 0;
}