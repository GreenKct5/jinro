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

char * strRole(Role role) {
    switch (role) {
        case VILLAGER:
            return "村人";
        case WEREWOLF:
            return "人狼";
        case SEER:
            return "占い師";
        case THIEF:
            return "怪盗";
        default:
            return "";
    }
}

int randomRole(Player *players, Role *trash, int playerNum) {
    Role roleCards[8] = {WEREWOLF, WEREWOLF, SEER, THIEF, VILLAGER, VILLAGER, VILLAGER, VILLAGER};
    int useCardsNum = playerNum + 2;
    srand((unsigned int)time(NULL));

    for (int i = 0; i < useCardsNum; i++) {
        int j = rand() % useCardsNum;
        Role temp = roleCards[i];
        roleCards[i] = roleCards[j];
        roleCards[j] = temp;
    }

    for (int i = 0; i < playerNum; i++) {
        players[i].role = roleCards[i];
    }
    for (int i = 0; i < 2; i++) {
        trash[i] = roleCards[playerNum + i];
    }
    return 0;
}
