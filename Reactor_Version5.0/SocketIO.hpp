#ifndef SOCKETIO_HPP
#define SOCKETIO_HPP

#include <string>
using std::string;

class SocketIO{
public:
    SocketIO(int);  // 接收并保存一个现有的文件描述符（fd）
    ~SocketIO() = default;
    int sendn(const char *buffer, int len); // 发送数据
    int readline(char *buffer, int maxlen); // 读一行数据
    int recvPeek(char *buffer, int maxlen) const;   // 窥探内核缓冲区中的数据数量
    int sendWithPrefix(const string &message);  // 发送数据（以数据长度前缀的方式）
    string receiveWithPrefix(); // 接受数据（以数据长度前缀的方式）

private:
    int recvn(char *buffer, int len);   // 接受数据（被readline所用）

private:
    int _fd;
};

#endif