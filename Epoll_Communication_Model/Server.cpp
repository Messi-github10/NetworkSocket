#include <my_header.h>
#include <errno.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8000
#define MAX_EVENTS 100
#define BUFFER_SIZE 1024
#define EPOLL_TIMEOUT 5000

// 服务端
int main(int argc, char *argv[])
{
    // 1.socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("failed to get listen_fd");
        return EXIT_FAILURE;
    }
    printf(">> listen_fd is [%d].\n", listen_fd);

    // 设置套接字属性：网络地址可复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2.bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);
    int bind_ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0)
    {
        perror("failed to bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 3.listen
    int listen_ret = listen(listen_fd, 20000);
    if (listen_ret < 0)
    {
        perror("failed to listen");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 4.创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 5.epoll添加对listen_fd事件的监听
    // 监听目标​：是否有​新客户端连接进来
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN; // 监听读事件
    ev.data.fd = listen_fd;
    int epoll_ctl_ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
    if (epoll_ctl_ret < 0)
    {
        perror("epoll_ctl failed");
        close(epoll_fd);
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 6.事件循环
    struct epoll_event events[MAX_EVENTS] = {0};
    while (1)
    {
        // 将已经就绪的文件描述符信息全部写入该数组
        int ready_num = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_TIMEOUT);
        if (ready_num == -1 && errno == EINTR)
        {
            continue; // 被信号影响
        }
        else if (ready_num == -1) // 发生错误
        {
            perror("epoll_wait failed");
        }
        else if (ready_num == 0) // 超时
        {
            printf("epoll timeout.\n");
        }
        else
        {
            printf("[Server] epoll_wait returned [%d] event(s).\n", ready_num);

            for (int i = 0; i < ready_num; i++)
            {
                // cur_fd 可能是 listen_fd （一个），也可能是 net_fd（多个）
                // cur_fd == listen_fd的时候表示有新连接
                // cur_fd != listen_fd的时候表示已经建立好的连接上有读事件就绪
                int cur_fd = events[i].data.fd;
                if (cur_fd == listen_fd)
                {
                    // 6.1.处理新连接
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int net_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (net_fd < 0)
                    {
                        perror("failed to accepted");
                    }

                    // 6.2.新连接信息的打印
                    printf("connect_fd %d tcp %s:%d --> %s:%d has connected.\n",
                           net_fd,
                           inet_ntoa(server_addr.sin_addr),
                           ntohs(server_addr.sin_port),
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));

                    // 6.3.epoll对net_fd进行读事件的监听
                    // 已连接的客户端是否有数据发送过来
                    struct epoll_event client_ev;
                    memset(&client_ev, 0, sizeof(client_ev));
                    client_ev.data.fd = net_fd;
                    client_ev.events = EPOLLIN;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, net_fd, &client_ev);
                }
                else
                {
                    // cur_fd != listen_fd表示已经建立好的连接上有读事件就绪
                    char buffer[BUFFER_SIZE] = {0};
                    // 6.4.接收数据
                    int recv_ret = recv(cur_fd, buffer, sizeof(buffer) - 1, 0);
                    if (recv_ret <= 0)
                    {
                        if (recv_ret == 0)
                        {
                            printf("connect_fd [%d] has closed.\n", cur_fd);
                        }
                        else
                        {
                            perror("Server received message failed");
                        }
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cur_fd, NULL);
                        close(cur_fd);
                        continue;
                    }
                    // 6.5.1.连接正常的情况下
                    buffer[recv_ret] = '\0';
                    printf("Server received message is [%s] from Client [%d] bytes.\n", buffer, recv_ret);

                    // 6.5.2.服务器回显客户端的消息
                    int send_ret = send(cur_fd, buffer, strlen(buffer), 0);
                    if (send_ret < 0)
                    {
                        perror("Server send message failed");
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cur_fd, NULL);
                        close(cur_fd);
                        continue;
                    }
                    printf("Server sent [%d] bytes to Client.\n", send_ret);
                }
            }
        }
    }

    close(epoll_fd);
    close(listen_fd);
    return 0;
}