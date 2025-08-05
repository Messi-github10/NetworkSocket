#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

// 客户端
int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = connect(client_fd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret < 0)
    {
        perror("connect");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("Client has connected.\n");

    fd_set set;

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        FD_SET(client_fd, &set);

        select(client_fd + 1, &set, NULL, NULL, NULL);
        if (FD_ISSET(client_fd, &set))
        {
            char buf[60] = {0};
            ssize_t ret = recv(client_fd, buf, sizeof(buf), 0);
            if (ret == 0)
            {
                printf("Server has closed.\n");
                break;
            }
            printf("Client received message is [%s].\n", buf);
        }

        if (FD_ISSET(STDIN_FILENO, &set))
        {
            char buf[60] = {0};
            read(STDIN_FILENO, buf, sizeof(buf));
            int ret = send(client_fd, buf, strlen(buf), 0);
            if (buf[strlen(buf) - 1] == '\n')
            {
                buf[strlen(buf) - 1] = '\0';
            }
            printf("Client sent message is [%s] to Server [%d] bytes.\n", buf, ret);
        }
    }

    close(client_fd);

    return 0;
}