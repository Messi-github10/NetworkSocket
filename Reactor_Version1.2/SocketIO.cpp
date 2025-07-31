#include "SocketIO.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

SocketIO::SocketIO(int fd)
    : _fd(fd)
{
}

SocketIO::~SocketIO() = default;

// 确定发送 len 字节的数据
int SocketIO::sendn(const char *buffer, int len)
{
    int left = len;
    const char *pbuffer = buffer;
    while (left > 0)
    {
        int ret = send(_fd, pbuffer, left, 0);
        if (ret < 0)
        {
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
        if(ret == -1 && errno == EINTR){
            continue;
        }else if(ret == -1){
            perror("recv");
            return len - left;
        }else if(ret == 0){
            return len - left;
        }
        left -= ret;
        pbuffer += ret;
    }
    return len;
}

// 一次获取一行数据
// max表示的是一行数据的最大字节数
// 最后一个字节存放 '\0'
// ---------------------- 弃用 --------------------------
int SocketIO::readline(char *buffer, int max)
{
    int left = max - 1;
    char *pbuffer = buffer;
    int total = 0;
    while (left > 0)
    {
        int ret = recv(_fd, pbuffer, left, MSG_PEEK);

        // 在读取 ret 个字节中查找'\n'
        for (int i = 0; i < ret; i++)
        {
            if (pbuffer[i] == '\n')
            {
                // 找到 '\n'
                int size = i + 1;
                int recvn_ret = recvn(pbuffer, size);
                if (recvn_ret <= 0)
                {
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

// ---------------------- 使用长度前缀的方式 --------------------------
int SocketIO::sendWithPrefix(const string &message){
    uint32_t length = htonl(message.size());    // 转化成网络字节序（大端）

    // 先发送长度前缀（4B）
    int ret = sendn(reinterpret_cast<const char *>(&length), sizeof(length));
    if(ret != sizeof(length)){
        return -1;
    }
    return sendn(message.c_str(), message.size());
}

string SocketIO::receiveWithPrefix(){
    uint32_t length = 0;

    // 先接收长度前缀（4B）
    int ret = recvn(reinterpret_cast<char *>(&length), sizeof(length));
    if(ret != sizeof(length)){
        return "";
    }
    length = ntohl(length); // 转化成主机字节序

    // 接收实际数据
    char *buffer = new char[length + 1];
    ret = recvn(buffer, length);
    if(ret != static_cast<int>(length)){
        delete[] buffer;
        return "";
    }
    buffer[length] = '\0';
    string message(buffer, length);
    delete[] buffer;
    return message;
}

// --------------------------------------------------------------------