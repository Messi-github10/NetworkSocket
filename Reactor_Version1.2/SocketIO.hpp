#ifndef SOCKETIO_HPP
#define SOCKETIO_HPP

#include <string>
using std::string;

/*
SocketIO类：
该类的作用就是封装所有的读写操作，也就是将读写的细节封装到该类中。
封装read/recv函数以及write/send函数，将这些函数的细节封装到该类中。
*/

class SocketIO{
public:
    SocketIO(int);
    ~SocketIO();
    int sendn(const char *buffer, int len);
    int readline(char *buffer, int max);
    int sendWithPrefix(const string &message);
    string receiveWithPrefix();

private:
    int recvn(char *buffer, int len);

private:
    int _fd;
};

#endif