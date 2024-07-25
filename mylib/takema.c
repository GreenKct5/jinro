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

char * hello_takema() {
    return "Hello. I'm takema!";
}

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
