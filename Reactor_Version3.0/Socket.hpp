#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Noncopyable.hpp"

class Socket : public Noncopyable{
public:
    Socket();
    Socket(int);
    ~Socket();
    void shutDownWrite();
    int get_fd() const;

private:
    int _fd;
};

#endif
