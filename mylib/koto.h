#ifndef KOTO_H
#define KOTO_H
#include "./takema.h"

char *chop_newline(char *str, int len);
void displayPlayersName(int playerNum, Player *players);

void divination(Player* seer, Player* players, int playerNum);

void *voting(int playerNum, Player *players);
int VoteDisclosure(int *votes, int playerNum);

int selectVictim(Player* thief,Player* players,int playerNum);
int stealRole(Player* thief,Player* victim);
#endif /* KOTO_H */
