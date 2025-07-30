#include <my_header.h>

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

#if 0

    // 3.recv
    char recv_message[1024] = {0};
    int recv_ret = recv(client_fd, recv_message, sizeof(recv_message) - 1, 0);
    if (recv_ret < 0)
    {
        perror("Client failed to receive message");
        close(client_fd);
    }
    recv_message[recv_ret] = '\0';
    printf("Client received message is [%s] from Server [%d] bytes.\n", recv_message, recv_ret);

#endif

    // 4.send
    const char message[] = "Hello, I am Client!\n";
    int send_ret = send(client_fd, message, strlen(message), 0);
    printf("Client sent [%d] bytes.\n", send_ret);

    // 5.close
    close(client_fd);
    
    return 0;
}