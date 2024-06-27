#include <stdio.h>

#define MAX_NAME_LENGTH 15
char * hello_takema(){
    return "Hello. I'm takema!";
}

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
