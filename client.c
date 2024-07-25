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
#include <pthread.h>
#include "./mylib/hachi.h"
#include "./mylib/koto.h"
#include "./mylib/takema.h"

#define PORT (in_port_t) 50000
#define BUF_LEN 512

int main() {
    struct sockaddr_in server;
    int soc;
    char ip_str[BUF_LEN] = "127.0.0.1";
    struct in_addr ip_addr;
    char buf[BUF_LEN];
    char username[BUF_LEN];

    write(1, "名前を入力してください        : ", strlen("名前を入力してください        : "));
    read(0, username, BUF_LEN);
    chop_newline(username, BUF_LEN);

    inet_aton(ip_str, &ip_addr);
    memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memcpy((char *)&server.sin_addr, &ip_addr, sizeof(ip_addr));

    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (connect(soc, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        exit(1);
    }

    write(soc, username, strlen(username) + 1);
    write(1, "Go Ahead!\n", strlen("Go Ahead!\n"));

    // 役職メッセージを受信
    int n = read(soc, buf, BUF_LEN);
    if (n > 0) {
        buf[n] = '\0';
        write(1, buf, n);
    }

    fd_set readset, readset_origin;
    int fd = soc;

    FD_ZERO(&readset);
    FD_SET(0, &readset);
    FD_SET(fd, &readset);
    readset_origin = readset;

    do {
        readset = readset_origin;
        select(fd + 1, &readset, NULL, NULL, NULL);
        if (FD_ISSET(0, &readset)) {
            int n = read(0, buf, BUF_LEN);
            write(fd, buf, n);
        }
        if (FD_ISSET(fd, &readset)) {
            int n;
            n = read(fd, buf, BUF_LEN);
            write(1, buf, n);
        }
    } while (strncmp(buf, "quit", 4) != 0);

    close(soc);
}