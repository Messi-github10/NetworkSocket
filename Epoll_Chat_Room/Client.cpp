#include <my_header.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define BUF_SIZE 1024
#define RECONNECT_DELAY 3  // 重连等待时间（秒）

int connect_to_server(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(fd);
        return -1;
    }
    
    printf("Welcome to Client.\n");
    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);
    return fd;
    
}

int main(int argc, char *argv[]){
    int client_fd = -1;
    char buffer[BUF_SIZE] = {0};

    while(1){
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(client_fd, &read_set);
        FD_SET(STDIN_FILENO, &read_set);

        // 如果有socket则加入监听
        if(client_fd >= 0){
            FD_SET(client_fd, &read_set);
        }

        int max_fd = client_fd > STDIN_FILENO ? client_fd : STDERR_FILENO;
        int ready_num = select(max_fd + 1, &read_set, NULL, NULL, NULL);

        if(ready_num < 0){
            perror("select error");
            break;
        }

        // 处理标准输入
        if(FD_ISSET(STDIN_FILENO, &read_set)){
            memset(buffer, 0, sizeof(buffer));
            int read_ret = read(STDIN_FILENO, buffer, sizeof(buffer));
            if(read_ret <= 0){
                perror("stdin closed");
                continue;
            }

            // 去掉换行符
            buffer[strcspn(buffer, "\n")] = '\0';

            // 未建立连接的话，则当标准输入的时候再重新连接
            if(client_fd < 0){
                client_fd = connect_to_server();
                if(client_fd < 0){
                    printf("Failed to reconnect. Try again later.\n");
                    continue;  // 不发送消息，等待下次输入
                }
            }

            // 发送数据
            int send_ret = send(client_fd, buffer, strlen(buffer), 0);
            if (send_ret < 0)
            {
                perror("send error");
                close(client_fd);
                client_fd = -1;
                continue;
            }
            printf("Client send message is [%s] to Server [%d] bytes.\n", buffer, send_ret);
        }

        // 处理服务器消息
        if(FD_ISSET(client_fd, &read_set)){
            memset(buffer, 0, sizeof(buffer));
            int recv_ret = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if(recv_ret <= 0){
                if(recv_ret == 0){
                    printf("Server disconnected.\n");
                }else{
                    perror("recv error");
                }
                close(client_fd);
                client_fd = -1;
                continue;
            }
            printf("Client receive message is [%s] from Server [%d] bytes.\n", buffer, recv_ret);
        }
    }

    if(client_fd >= 0){
        close(client_fd);
    }
    return 0;
}