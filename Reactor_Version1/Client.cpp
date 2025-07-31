#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");//代表本机地址

    int ret = connect(clientfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(ret < 0) {
        perror("connect");
        close(clientfd);
        return EXIT_FAILURE;
    }

    printf("connect success\n");

    //通过select监听clientfd和标准输入
    fd_set readset;
    FD_ZERO(&readset);
    char buff[1024] = {0};

    while(1) {
        FD_ZERO(&readset);
        FD_SET(clientfd, &readset);
        FD_SET(STDIN_FILENO, &readset);

        int nready = select(clientfd + 1, &readset, NULL, NULL, NULL);
        printf("select nready %d\n", nready);

        if(FD_ISSET(STDIN_FILENO, &readset)) {
            //监听到了标准输入
            memset(buff, 0, sizeof(buff));//注意: 要经常做,防止有一些脏数据影响结果
            //注意：使用read接收数据时，会包含'\n'
            ret = read(STDIN_FILENO, buff, sizeof(buff));
            printf("read %d bytes.\n", ret);
            //发送给服务器
            ret = send(clientfd, buff, strlen(buff), 0);
            printf("send %d bytes.\n", ret);
        } 

        if(FD_ISSET(clientfd, &readset)) {
            //服务器给客户端发数据了
            memset(buff, 0, sizeof(buff));//注意: 要经常做,防止有一些脏数据影响结果
            ret = recv(clientfd, buff, sizeof(buff), 0);
            if(ret == 0) {
                break;
            }
            printf("recv from server: %s, %d bytes.\n", buff, ret);
        }
    }
    //断开连接
    close(clientfd);
    return 0;
}
