#include "SocketIO.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>

SocketIO::SocketIO(int fd)
:_fd(fd)
{}

int SocketIO::sendn(const char *buffer, int len){
    int left = len;
    const char *pbuffer = buffer;
    while(left > 0){
        int ret = send(_fd, buffer, left, 0);
        if(ret < 0){
            perror("send");
            return len - left;
        }
        left -= ret;
        pbuffer += ret;
    }
    return len;
}

int SocketIO::recvPeek(char *buffer, int maxlen) const{
    int ret = 0;
    do{
        ret = recv(_fd, buffer, maxlen, MSG_PEEK);
    } while (ret == -1 && errno == EINTR);

    if(ret < 0){
        perror("recv");
    }
    return ret;
}

int SocketIO::readline(char *buffer, int maxlen){
    int left = maxlen - 1;
    char *pbuffer = buffer;
    int total = 0;
    while(left > 0){
        int ret = recvPeek(buffer, maxlen);
        int total = 0;
        // 在窥视的ret个字节中查找'\n'
        for (int i = 0; i < ret;i++){
            if(pbuffer[i] == '\n'){
                // 找到了'\n'
                int size = i + 1;
                ret = recvn(pbuffer, size);
                pbuffer[i] = '\0';
                total += i;
                return total;
            }
        }
        // 没有找到'\n'，从缓冲区中刷走
        ret = recvn(pbuffer, ret);
        left -= ret;
        pbuffer += ret;
        total += ret;
    }
    buffer[maxlen - 1] = '\0';
    return maxlen - 1;
}

int SocketIO::sendWithPrefix(const string &message){
    uint32_t length = htonl(message.size());
    int ret = sendn(reinterpret_cast<const char *>(&length), sizeof(length));
    if(ret != sizeof(length)){
        return -1;
    }
    return sendn(message.c_str(), message.size());
}

string SocketIO::receiveWithPrefix(){
    uint32_t length = 0;
    int ret = recvn(reinterpret_cast<char *>(&length), sizeof(length));
    if(ret != sizeof(length)){
        return "";
    }
    length = ntohl(length);
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

int SocketIO::recvn(char *buffer, int len){
    int left = len;
    char *pbuffer = buffer;
    while(left > 0){
        int ret = recv(_fd, pbuffer, left, 0);
        if(ret == -1 && errno == EINTR){
            continue;
        }else if(ret == 0){
            // 连接断开
            return len - left;
        }else if(ret == -1){
            perror("recv");
            return len - left;
        }
        left -= ret;
        pbuffer += ret;
    }
    return len;
}