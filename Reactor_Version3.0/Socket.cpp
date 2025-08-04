#include "Socket.hpp"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

Socket::Socket(){
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_fd < 0){
        perror("socket");
    }
}

Socket::Socket(int fd)
:_fd(fd)
{}

Socket::~Socket(){
    close(_fd);
    printf("socket %d has closed.\n", _fd);
}

void Socket::shutDownWrite(){
    shutdown(_fd, SHUT_WR);
}

int Socket::get_fd() const{
    return _fd;
}