#include <stdio.h>
#include <string.h>

char * hello_koto() {
    return "Hello. I'm koto!";
}

char * chop_newline(char *str,int len){
    int n = strlen(str);
    if(n<len && str[n-1]=='\n'){
        str[n-1] = '\0';
    }
    return str;
}