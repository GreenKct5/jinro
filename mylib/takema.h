#ifndef TAKEMA_H
#define TAKEMA_H
#define MAX_NAME_LENGTH 15
typedef enum {
    VILLAGER,
    WEREWOLF,
    SEER, 
    THIEF,
} Role;

typedef struct {
    char name[MAX_NAME_LENGTH];
    Role role;
    int sock;
} Player;

char * hello_takema();
char * strRole(Role role);
int randomRole(Player *players,Role* trash,int playerNum);
int stealRole(Player* thief,Player* victim);
#endif /* TAKEMA_H */

