#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void* moveCursorUp(void* arg) {
    int* params = (int*)arg;
    int lines = params[0];
    int minutes = params[1];
    int seconds = params[2];
    time_t start, current;
    int total_seconds = minutes * 60 + seconds; // 指定された時間を秒に変換
    time(&start); // タイマーの開始時間を取得

    printf("\n"); // タイマーを表示する行に移動

    while (1) {
        time(&current); // 現在の時間を取得
        int elapsed = difftime(current, start); // 経過時間を計算
        int remaining = total_seconds - elapsed; // 残り時間を計算

        if (remaining < 0) {
            printf("時間切れ\n");
            printf("終了\n"); // 次の行に終了と表示
            break;
        }

        int remaining_minutes = remaining / 60;
        int remaining_seconds = remaining % 60;

        printf("残り時間: %02d分%02d秒", remaining_minutes, remaining_seconds); // 残り時間を表示
        fflush(stdout); // 出力をフラッシュして即時表示させる

        printf("\033[%dA", lines); // タイマーを表示する行にカーソルを移動
        printf("\r\033[K"); // 行をクリア

        sleep(1); // 1秒待機
        printf("\n"); // 次の行に移動（チャットルームの表示など他の出力のため）
    }
    return NULL;
}

void chatroom_kari(){
    sleep(2); // 1秒待機
    printf("[koto]占いCOお願いします。\n"); // 仮のチャット
    sleep(3); // 1秒待機
    printf("[noname]自分占いです。\n"); // 仮のチャット
    sleep(5); // 1秒待機
    printf("[takema]占い対抗出ます。\n"); // 仮のチャット
}

int main() {
    pthread_t timer_thread;
    int params[3] = {1, 1, 30}; // 移動する行数、分、秒

    // タイマーを別スレッドで起動
    pthread_create(&timer_thread, NULL, moveCursorUp, (void*)params);

    // 他のプログラムの処理
    chatroom_kari();

    // タイマーのスレッドが終了するのを待つ
    pthread_join(timer_thread, NULL);

    return 0;
}