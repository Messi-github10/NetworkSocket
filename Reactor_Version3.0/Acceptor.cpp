#include "Acceptor.hpp"
#include <string.h>

#define BACKLOG 20000

Acceptor::Acceptor(unsigned short port, const string &ip)
:_addr(port, ip)
,_listen_sock()
{}


void Acceptor::ready(){
    setReuseAddr(true);
    setReusePort(true);
    bind();
    listen();
}

int Acceptor::accept(){
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    socklen_t len = sizeof(peer_addr);
    int peer_fd = ::accept(_listen_sock.get_fd(), (struct sockaddr *)&peer_addr, &len);
    if(peer_fd < 0){
        perror("accept");
    }
    return peer_fd;
}

int Acceptor::get_listen_fd() const{
    return _listen_sock.get_fd();
}

void Acceptor::setReuseAddr(bool opt){
    int op = opt > 0 ? 1 : 0;
    int ret = setsockopt(_listen_sock.get_fd(), SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
    if(ret < 0){
        perror("setsockopt");
    }
}

void Acceptor::setReusePort(bool opt){
    int op = opt > 0 ? 1 : 0;
    int ret = setsockopt(_listen_sock.get_fd(), SOL_SOCKET, SO_REUSEPORT, &op, sizeof(op));
    if(ret < 0){
        perror("setsockopt");
    }
}

void Acceptor::bind(){
    int ret = ::bind(_listen_sock.get_fd(), (struct sockaddr *)&_addr, sizeof(_addr));
    if(ret < 0){
        perror("bind");
    }
}

void Acceptor::listen(){
    int ret = ::listen(_listen_sock.get_fd(), BACKLOG);
    if(ret < 0){
        perror("listen");
    }
}