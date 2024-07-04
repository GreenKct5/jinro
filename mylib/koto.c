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

#define BUF_LEN             512

char * hello_koto() {
    return "Hello. I'm koto!";
}

// 配列から改行ををなくす
char * chop_newline(char *str,int len){
    int n = strlen(str);
    if(n<len && str[n-1]=='\n'){
        str[n-1] = '\0';
    }
    return str;
}

// 投票
void voting(const char *username) {
    char buf[BUF_LEN];
    char *userList[] = {"koto", "takema", "noname", "hachi"};

    write(1, "誰に投票しますか\n", strlen("誰に投票しますか\n"));
    int i = 0;
    int length = sizeof(userList) / sizeof(userList[0]);
    char option[64];
    int optionIndex = 0;

    while (i < length) {
        if (strcmp(userList[i], username) != 0) {
            snprintf(option, sizeof(option), "%c. %s\n", 'a' + optionIndex, userList[i]); // optionにフォーマットした文字列を格納する
            write(1, option, strlen(option));
            optionIndex++;
        }
        i++;
    }
    snprintf(option, sizeof(option), "%c. この中に人狼はいない\n", 'a' + optionIndex);
    write(1, option, strlen(option));
    write(1, "入力: ", strlen("入力: "));
    read(0, buf, BUF_LEN);
}
