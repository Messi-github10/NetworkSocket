#include <my_header.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8000

// 服务器端
int main(int argc, char *argv[])
{
    // 1.socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("failed to get listen_fd");
        return EXIT_FAILURE;
    }

    // 2.bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                     // IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR); // 点分十进制
    server_addr.sin_port = htons(SERVER_PORT);            // 短整型（16bit）

    int bind_ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0)
    {
        perror("failed to bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 3.listen
    int listen_ret = listen(listen_fd, 1);
    if (listen_ret < 0)
    {
        perror("failed to listen");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_addr_len = sizeof(client_addr);

        // 4.accept
        int net_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (net_fd < 0)
        {
            perror("failed to accept");
            close(listen_fd);
            continue;
        }

        // 5.send
        const char message[] = "Welcome to connect Server";
        int send_ret = send(net_fd, message, strlen(message), 0);
        printf("Server sent [%d] bytes.\n", send_ret);

        // 6.recv
        char recv_message[1024] = {0};
        int recv_ret = recv(net_fd, recv_message, sizeof(recv_message) - 1, 0);
        if (recv_ret < 0)
        {
            perror("Server failed to receive message");
            close(net_fd);
            continue;
        }
        recv_message[recv_ret] = '\0';
        printf("Server received message is [%s] from Client [%d] bytes.\n", recv_message, recv_ret);

        // 7.close
        close(net_fd);
    }

    // 8.close
    close(listen_fd);

    return 0;
}