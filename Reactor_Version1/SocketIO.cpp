#include "SocketIO.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

SocketIO::SocketIO(int fd)
:_fd(fd)
{}

SocketIO::~SocketIO() = default;

// 确定发送 len 字节的数据
int SocketIO::sendn(const char *buffer, int len){
    int left = len;
    const char *pbuffer = buffer;
    while(left > 0){
        int ret = send(_fd, pbuffer, left, 0);
        if(ret < 0){
            perror("send");
            return len - left;
        }
        left -= ret;
        pbuffer += ret;
    }
    return len;
}

// 确定接收 len 字节的数据
int SocketIO::recvn(char *buffer, int len){
    int left = len;
    char *pbuffer = buffer;
    while(left > 0){
        int ret = recv(_fd, pbuffer, left, 0);
        if(ret < 0){
            perror("recv");
            return len - ret;
        }
        left -= ret;
        pbuffer += ret;
    }
    return len;
}

// 一次获取一行数据
// max表示的是一行数据的最大字节数
// 最后一个字节存放 '\0'
int SocketIO::readline(char *buffer, int max){
    int left = max - 1;
    char *pbuffer = buffer;
    int total = 0;
    while (left > 0)
    {
        int ret = recv(_fd, pbuffer, left, MSG_PEEK);

        // 在读取 ret 个字节中查找'\n'
        for(int i = 0; i < ret; i++){
            if(pbuffer[i] == '\n'){
                // 找到 '\n'
                int size = i + 1;
                int recvn_ret = recvn(pbuffer, size);
                if(recvn_ret <= 0){
                    perror("recvn");
                    return ret;
                }
                pbuffer[i] = '\0';
                total += i;
                return total;
            }
        }
        // 未能找到 '\n'
        // 从内核缓冲区中刷走
        ret = recvn(pbuffer, ret);
        left -= ret;
        pbuffer += ret;
    }
    return max - 1;
}