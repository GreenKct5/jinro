#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

char * hello_hachi(){
    return "Hello. I'm hachi!";
}

// int create_server_socket(struct sockaddr_in *me);
// void accept_clients(int server_socket, int *client_sockets, int playerNum);
// void receive_and_broadcast(int *client_sockets, int playerNum);

// int main(){
//     struct sockaddr_in me;
//     int playerNum = 0;
//     int server_socket;
//     server_socket = create_server_socket(&me);

//     int client_sockets[playerNum];
//     accept_clients(server_socket, client_sockets, playerNum);

//     close(server_socket);

//     receive_and_broadcast(client_sockets, playerNum);

//     for (int i = 0; i < playerNum; i++){
//         close(client_sockets[i]);
//     }
//     return 0;
// }

// int create_server_socket(struct sockaddr_in *me){
//     int server_socket;
//     if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
//         perror("socket");
//         exit(1);
//     }

//     if (bind(server_socket, (struct sockaddr *)me, sizeof(*me)) == -1){
//         perror("bind");
//         exit(1);
//     }
//     listen(server_socket, 5);
//     return server_socket;
// }

// void accept_client(int server_socket, int *client_sockets, int playerNum){
//     for (int i = 0; i < playerNum; i++){
//         client_sockets[i] = accept(server_socket, NULL, NULL);
//         if (client_sockets[i] == -1){
//             perror("accept");
//             exit(1);
//         }
//     }
// }

// void receive_and_broadcast(int *client_sockets, int playerNum){
//     char buf[BUF_LEN];
//     char names[playerNum][BUF_LEN];
//     fd_set readset, readset_origin;
//     FD_ZERO(&readset);
//     for (int i = 0; i < playerNum; i++){
//         FD_SET(client_sockets[i], &readset);
//     }
//     readset_origin = readset;

//     for (int i = 0; i < playerNum; i++){
//         int n = recv(client_sockets[i], buf, BUF_LEN, 0);
//         if (n > 0){
//             buf[n] = '\0';
//             strcpy(names[i], buf);
//         }
//     }

//     for (int i = 0; i < playerNum; i++){
//         for (int j = 0; j < playerNum; j++){
//             send(client_sockets[i], names[j], strlen(names[j]), 0);
//         }
//     }

//     do {
//         readset = readset_origin;
//         select(client_sockets[playerNum - 1] + 1, &readset, NULL, NULL, NULL);
//         for (int i = 0; i < playerNum; i++){
//             if (FD_ISSET(client_sockets[i], &readset)){
//                 int n =recv(client_sockets[i], buf, BUF_LEN, 0);
//                 for (int j = 1; j < playerNum; j++){
//                     send(client_sockets[(i + j) % playerNum], buf, n, 0);
//                 }
//                 write(1, buf, n);
//             }
//         }
//     } while(strncmp(buf,"quit",4) != 0);
// }