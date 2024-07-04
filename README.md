# C言語で作る人狼ゲーム
## プロジェクトのクローン手順
1. 作業フォルダを用意し，ターミナルで移動
2. cloneする
```bash
git clone git@github.com:GreenKct5/jinro.git
```
## コンパイルの仕方
1. mylibフォルダに移動. mylibフォルダ内のファイルをコンパイル
```bash
cd mylib
make
```
2. 戻ってclientとserverのコンパイルとヘッダファイルとリンクをさせる
```bash
cd ..
make
```
