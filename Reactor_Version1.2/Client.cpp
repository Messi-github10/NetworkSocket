#include "SocketIO.hpp"
#include <string>
#include <my_header.h>
using std::string;

#define SERVER_ADDR "0.0.0.0"
#define SERVER_PORT 8000

// 客户端
int main(int argc, char *argv[]){
    // 1.socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("failed to get client_fd");
        return EXIT_FAILURE;
    }

    // 2.connect
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    client_addr.sin_port = htons(SERVER_PORT);

    int connect_ret = connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if(connect_ret < 0){
        perror("failed to connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Connect Success.\n");

    SocketIO socketIO(client_fd);

    // 3.send
    const char message[] = "Hello, I am Client!";
    socketIO.sendWithPrefix(message);
    printf("Client sent message with prefix: [%s]\n", message);

    // 4.recv
    string reply_message = socketIO.receiveWithPrefix();
    printf("Client received reply with prefix: [%s]\n", reply_message.c_str());

    // 5.close
    close(client_fd);
    
    return 0;
}