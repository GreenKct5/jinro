#include <stdio.h>
#include <time.h>
#include <unistd.h>

int LimitTime = 10; // タイマーの制限時間   

char * hello_noname(){
    return "Hello. I'm noname!";
}

void chatroom_kari(){
    printf("[koto]占いCOお願いします。\n"); // 仮のチャット
    printf("[noname]了解しました。\n"); // 仮のチャット
}

#include <stdio.h>
#include <time.h>
#include <unistd.h>

void moveCursorUp(int lines, int minutes, int seconds) {
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
}

int main() {
    chatroom_kari(); // 仮のチャットルーム
    moveCursorUp(1, 1, 30); // 移動する行数、分、秒
    return 0;
}