#ifndef KOTO_H
#define KOTO_H
#include "./takema.h"

char *chop_newline(char *str, int len);
void divination(Player* seer, Player* players, int playerNum);
void *voting(int playerNum, Player *players);
int VoteDisclosure(int *votes, int playerNum);
#endif /* KOTO_H */
