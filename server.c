#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#include "./mylib/voting.h"
#include "./mylib/role.h"

#define PORT (in_port_t) 50000
#define BUF_LEN 512

// グローバル変数を定義
typedef void (*end_game_callback_t)(void);
static int global_playerNum;
static Player *global_players;

// コールバックラッパー関数
void endGameWrapper(void) {
    voting(global_playerNum, global_players);
}

static end_game_callback_t end_game_callback = NULL;
void set_end_game_callback(end_game_callback_t callback) {
    end_game_callback = callback;
}

void* timer(void* arg) {
    int* params = (int*)arg;
    int client_count = params[0];
    int minutes = params[1];
    int seconds = params[2];
    int* socks = &params[3];
    time_t start, current;
    int total_seconds = minutes * 60 + seconds; // 指定された時間を秒に変換
    time(&start); // タイマーの開始時間を取得

    char buffer[1024];

    while (1) {
        time(&current); // 現在の時間を取得
        int elapsed = difftime(current, start); // 経過時間を計算
        int remaining = total_seconds - elapsed; // 残り時間を計算

        if (remaining < 0) {
            snprintf(buffer, sizeof(buffer), "\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n時間切れ\n話し合いを終了して下さい。\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            for (int i = 0; i < client_count; i++) {
                if (send(socks[i], buffer, strlen(buffer), 0) < 0) {
                    perror("send");
                }
            }
            printf("%s", buffer); // サーバにも表示

            // コールバックを呼び出す
            if (end_game_callback) {
                end_game_callback();
            }
            
            break;
        } else if (remaining % 30 == 0) {
            int remaining_minutes = remaining / 60; // 残り分数を計算
            int remaining_seconds = remaining % 60; // 残り秒数を計算
            snprintf(buffer, sizeof(buffer), "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n残り時間は、%02d分%02d秒 です。\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", remaining_minutes, remaining_seconds);
            for (int i = 0; i < client_count; i++) {
                if (send(socks[i], buffer, strlen(buffer), 0) < 0) {
                    perror("send");
                }
            }
            printf("%s", buffer); // サーバにも表示
            sleep(1); // 1秒待機
        } else {
            sleep(1); // 1秒待機
        }
    }

    for (int i = 0; i < client_count; i++) {
        if (close(socks[i]) < 0) {
            perror("close");
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in me;
    int playerNum = 4;
    int soc_waiting;
    char buf[BUF_LEN];

    write(1, "このゲームは4人プレイです\n", strlen("このゲームは4人プレイです\n"));

    // タイマーの分と秒を設定
    int minutes = 2;
    int seconds = 30;

    memset((char *)&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port = htons(PORT);

    if ((soc_waiting = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(soc_waiting, (struct sockaddr *)&me, sizeof(me)) == -1) {
        perror("bind");
        exit(1);
    }

    listen(soc_waiting, playerNum);
    Player players[playerNum];
    for (int i = 0; i < playerNum; i++) {
        players[i].sock = accept(soc_waiting, NULL, NULL);
        read(players[i].sock, players[i].name, BUF_LEN);
    }

    displayPlayersName(playerNum, players);

    int client_socks[playerNum + 3];
    client_socks[0] = playerNum;
    client_socks[1] = minutes;
    client_socks[2] = seconds;
    for (int i = 0; i < playerNum; i++) {
        client_socks[i + 3] = players[i].sock;
    }

    Role trash[2];
    randomRole(players, trash, playerNum);

    for (int i = 0; i < playerNum; i++) {
        snprintf(buf, BUF_LEN, "\nあなたの役職は %s です\n", strRole(players[i].role));
        write(players[i].sock, buf, strlen(buf));
        if (players[i].role == WEREWOLF) checkwolf(&players[i], players, playerNum);
        if (players[i].role == SEER) divination(&players[i], players, playerNum);
    }
    for (int i = 0; i < playerNum; i++) {
        if (players[i].role == THIEF) selectVictim(&players[i], players, playerNum);
    }

    // グローバル変数に設定
    global_playerNum = playerNum;
    global_players = players;

    // コールバックの設定
    set_end_game_callback(endGameWrapper);


    // タイマーを別スレッドで起動
    pthread_t timer_thread;
    int result = pthread_create(&timer_thread, NULL, timer, (void*)client_socks);
    if (result != 0) {
        fprintf(stderr, "Failed to create timer thread: %s\n", strerror(result));
        exit(1);
    }

    for (int i = 0; i < playerNum; i++) {
        write(players[i].sock, "おはようございます．朝になりました\n話し合いを初めてください\n", strlen("おはようございます．朝になりました\n話し合いを初めてください\n"));
    }
    
    close(soc_waiting);

    fd_set readset, readset_origin;
    FD_ZERO(&readset);
    for (int i = 0; i < playerNum; i++) FD_SET(players[i].sock, &readset);
    readset_origin = readset;

    do {
        readset = readset_origin;
        select(players[playerNum - 1].sock + 1, &readset, NULL, NULL, NULL);
        for (int i = 0; i < playerNum; i++) {
            if (FD_ISSET(players[i].sock, &readset)) {
                int n = read(players[i].sock, buf, BUF_LEN);
                if (n > 0) {
                    buf[n] = '\0';

                    time_t now = time(NULL);
                    struct tm *t = localtime(&now);
                    char time_str[256];
                    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

                    char message[BUF_LEN + 256];
                    snprintf(message, sizeof(message), "(%s: %s) -> %s", players[i].name, time_str, buf);
                    for (int j = 0; j < playerNum; j++) {
                        if (j != i) {
                            write(players[j].sock, message, strlen(message));
                        }
                    }
                }
            }
        }
    } while (strncmp(buf, "quit", 4) != 0);

    for (int i = 0; i < playerNum; i++) close(players[i].sock);
    pthread_join(timer_thread, NULL);

    return 0;
}
