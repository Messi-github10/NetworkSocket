#include "TcpConnection.hpp"
#include <string.h>
#include <sys/socket.h>
#include <sstream>
using std::ostringstream;

TcpConnection::TcpConnection(int fd)
    : _sock(fd), _local_addr(getLocalAddress()), _peer_addr(getPeerAddress()), _socketIO(fd)
{
}

TcpConnection::~TcpConnection() = default;

void TcpConnection::send(const string &message)
{
    _socketIO.sendn(message.c_str(), message.size());
}

string TcpConnection::receive()
{
    char buffer[65536] = {0};
    int ret = _socketIO.readline(buffer, sizeof(buffer));
    if (ret <= 0)
    {
        return "";
    }
    return string(buffer, ret);
}

string TcpConnection::toString() const
{
    ostringstream oss;
    oss << "> TCP Socket " << _sock.fd()
        << ", " << _local_addr.ip() << ":" << _local_addr.port()
        << " -> " << _peer_addr.ip() << ":" << _peer_addr.port();
    return oss.str();
}

void TcpConnection::shutdown()
{
    _sock.shutDownWrite();
}

InetAddress TcpConnection::getLocalAddress() const
{
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    socklen_t len = sizeof(local_addr);
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&local_addr, &len);
    if (ret < 0)
    {
        perror("getsockname");
    }
    return InetAddress(local_addr);
}

InetAddress TcpConnection::getPeerAddress() const
{
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    socklen_t len = sizeof(peer_addr);
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&peer_addr, &len);
    if (ret < 0)
    {
        perror("getpeername");
    }
    return InetAddress(peer_addr);
}

void TcpConnection::sendWithPrefix(const string &message){
    _socketIO.sendWithPrefix(message);
}
string TcpConnection::receiveWithPrefix(){
    return _socketIO.receiveWithPrefix();
}
