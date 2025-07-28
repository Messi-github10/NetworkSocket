#include <my_header.h>

#define SERVER_ADDR "127.0.0.1"
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
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);
    int connect_ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(connect_ret < 0){
        perror("failed to connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Connect Success.\n");

    sleep(1);

    // 3.send
    const char *message = "Hello, I am Client!";
    int send_ret = send(client_fd, message, strlen(message), 0);
    if(send_ret < 0){
        perror("failed to send");
        close(client_fd);
        return EXIT_FAILURE;
    }
    printf("Client sent [%d] bytes.\n", send_ret);

    // 4.recv
    char buffer[1024] = {0};
    int recv_ret = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if(recv_ret < 0){
        perror("failed to recv");
        close(client_fd);
        return EXIT_FAILURE;
    }
    printf("Client receive message is [%s] from Server [%d] bytes", buffer, recv_ret);

    // 5.close
    close(client_fd);
    return 0;
}