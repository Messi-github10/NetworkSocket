#include <my_header.h>
#include <errno.h>

#define MAX_CLINTES 100
#define TIMEOUT 30

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define MAX_EVENTS 100
#define BUF_SIZE 1024

typedef struct
{
    int client_fd;        // 客户端文件描述符
    time_t last_active;   // 最后活跃时间
    char client_name[32]; // 客户端昵称
} Client;

Client clients[MAX_CLINTES];
int client_count = 0;
time_t last_timeout_check = 0;  // 记录上一次检查超时的时间

void set_nonblocking(int fd);                               // 设置套接字为非阻塞
void add_client(int fd);                                    // 添加客户端到服务端服务列表中
void remove_client(int fd, int epoll_fd);                   // 移除客户端
void broadcast_message(int sender_fd, const char *message); // 广播消息
void check_timeout_clients(int epoll_fd);                   // 超时剔除

// 服务端
int main(int argc, char *argv[])
{
    printf("Welcome to Server.\n");

    // 1.socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    // 设置套接字属性：网络地址可复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2.bind
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);
    int bind_ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0)
    {
        perror("bind failed");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 3.listen
    int listen_ret = listen(listen_fd, MAX_CLINTES);
    if (listen_ret < 0)
    {
        perror("listen failed");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    printf("Server started on %s:%d.\n", SERVER_IP, SERVER_PORT);

    // 4.创建epoll的实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        close(listen_fd);
        return EXIT_FAILURE;
    }

    // 5.监听 listen_fd 的读事件（是否有新连接）
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
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
        int timeout_ms = 1000;
        int ready_num = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout_ms);
        if (ready_num == -1 && errno == EINTR)
        {
            continue; // 被信号影响
        }
        else if (ready_num == -1)
        { // 发生错误
            perror("epoll_wait failed");
            break;
        }
        else if (ready_num == 0)
        { 
            // 超时
            //在1秒之内没有任何文件描述符就绪时，才会进入该分支
            //但如果有fd就绪，就不会进入该分支
        }
        else
        {
            printf("[Server] epoll_wait returned [%d] event(s).\n", ready_num);
            for (int i = 0; i < ready_num; i++)
            {
                int cur_fd = events[i].data.fd;

                // 检查错误事件
                if (events[i].events & (EPOLLERR | EPOLLHUP))
                {
                    remove_client(cur_fd, epoll_fd);
                    continue;
                }

                // 6.1.处理新连接
                if (cur_fd == listen_fd)
                {
                    struct sockaddr_in client_addr;
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                    if (client_fd < 0)
                    {
                        perror("accept failed");
                        continue;
                    }

                    // 6.1.1.设置为非阻塞IO
                    set_nonblocking(client_fd);

                    // 6.1.2.添加客户端到epoll（ET模式）
                    ev.events = EPOLLIN | EPOLLET; // 边缘触发
                    ev.data.fd = client_fd;
                    epoll_ctl_ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                    if (epoll_ctl_ret < 0)
                    {
                        perror("epoll_ctl failed");
                        close(client_fd);
                        continue;
                    }

                    add_client(client_fd);
                    printf("New connection from %s:%d (fd = %d).\n",
                           inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port),
                           client_fd);
                }
                else
                {
                    // 6.2.处理客户端数据（已经建立连接）
                    // 在 EPOLLET（边缘触发）模式 下，epoll_wait 只会通知一次某个文件描述符（fd）的可读/可写事件，即使缓冲区中仍有未读完的数据。
                    // 因此，必须循环读取直到数据被完全处理，否则会导致数据残留或丢失。
                    char buffer[BUF_SIZE] = {0};
                    while (1)
                    {
                        int len = recv(cur_fd, buffer, sizeof(buffer) - 1, 0);
                        buffer[len] = '\0';
                        if (len == 0)
                        { // 客户端主动关闭连接
                            remove_client(cur_fd, epoll_fd);
                            break;
                        }
                        else if (len < 0)
                        { // 服务器接收数据的时候发生错误
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                            {
                                break;
                            }
                            perror("recv failed");
                            remove_client(cur_fd, epoll_fd);
                            break;
                        }
                        else
                        {
                            // 正常情况
                            buffer[len] = '\0';
                            printf("Received from %d:%s.\n", cur_fd, buffer);

                            // 更新活跃时间
                            for (int i = 0; i < client_count; i++)
                            {
                                if (clients[i].client_fd == cur_fd)
                                {
                                    clients[i].last_active = time(NULL);
                                    break;
                                }
                            }

                            // 广播消息
                            broadcast_message(cur_fd, buffer);
                        }
                    }
                }
            }
        }
        check_timeout_clients(epoll_fd);
    }

    // 7.close
    close(epoll_fd);
    close(listen_fd);
    return 0;
}

// 设置套接字为非阻塞
void set_nonblocking(int fd)
{
    // 获取文件状态标志
    int flags = fcntl(fd, F_GETFL, 0);

    // 设置文件状态标志，通过位或操作（|）在原有标志位的基础上添加 O_NONBLOCK 标志。
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 添加客户端到服务端服务列表中
void add_client(int fd)
{
    if (client_count >= MAX_CLINTES)
    {
        fprintf(stderr, "Too many clients!\n");
        close(fd);
        return;
    }
    clients[client_count].client_fd = fd;
    clients[client_count].last_active = time(NULL);
    snprintf(clients[client_count].client_name, 32, "Client-%d", fd);
    client_count++;
    printf("New client: %s (fd = %d).\n", clients[client_count - 1].client_name, fd);
}

// 移除客户端
void remove_client(int fd, int epoll_fd)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].client_fd == fd)
        {
            printf("Kick client: %s (fd = %d).\n", clients[i].client_name, fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            close(fd);

            // 从服务端服务列表中删除
            clients[i] = clients[client_count - 1]; // 最后一个位置的元素填充
            client_count--;
            break;
        }
    }
}

// 广播消息（排除发送者）
void broadcast_message(int sender_fd, const char *message)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].client_fd != sender_fd)
        {
            send(clients[i].client_fd, message, strlen(message), 0);
        }
    }
}

// 超时剔除
void check_timeout_clients(int epoll_fd)
{
    time_t now = time(NULL);
    for (int i = client_count - 1; i >= 0; i--)
    {
        if (now - clients[i].last_active > TIMEOUT) {
            printf("Kicking client %s (fd=%d) due to inactivity.\n",
                  clients[i].client_name, clients[i].client_fd);
            remove_client(clients[i].client_fd, epoll_fd);
        }
    }
}