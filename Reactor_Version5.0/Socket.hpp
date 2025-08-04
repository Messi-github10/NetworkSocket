#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Noncopyable.hpp"

class Socket : public Noncopyable{
public:
    Socket();
    Socket(int);
    ~Socket();
    int get_fd() const;
    void shutDownWrite();

private:
    int _fd;
};

#endif