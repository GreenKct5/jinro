#include <stdio.h>
#include <stdlib.h>
#include "takema.h"
#define MAX_NAME_LENGTH 15


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
