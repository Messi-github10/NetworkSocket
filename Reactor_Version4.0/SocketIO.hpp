#ifndef SOCKETIO_HPP
#define SOCKETIO_HPP

#include <string>
using std::string;

class SocketIO{
public:
    SocketIO(int);
    ~SocketIO() = default;
    int sendn(const char *buffer, int len);
    int readline(char *buffer, int maxlen);
    int recvPeek(char *buffer, int maxlen) const;
    int sendWithPrefix(const string &message);
    string receiveWithPrefix();

private:
    int recvn(char *buffer, int len);

private:
    int _fd;
};

#endif