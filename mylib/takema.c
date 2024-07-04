#include <stdio.h>
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
} Player;

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
