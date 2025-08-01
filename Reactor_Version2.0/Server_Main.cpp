#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include "EventLoop.hpp"

#include <iostream>
using namespace std;

void onConnection(TcpConnectionPtr tcp_connection_ptr){
    // 打印五元组信息
    cout << tcp_connection_ptr->toString() << "has connected." << endl;
}

// ---------------- 数据处理 -------------------
// 可扩展
void onMessage(TcpConnectionPtr tcp_connection_ptr){
    // 获取消息
    string message = tcp_connection_ptr->receive();
    cout << "message size is [" << message.size() << "]." << endl;
    cout << "message is [" << message << "]." << endl;

    // 解码 -> 计算 -> 编码（TODO）

    // 发送消息（广播）
    tcp_connection_ptr->broadcast(message, tcp_connection_ptr);
}

void onClose(TcpConnectionPtr tcp_connection_ptr){
    cout << tcp_connection_ptr->toString() << "has closed." << endl;
}

int main(){
    Acceptor acceptor(8000);
    acceptor.ready();
    EventLoop event_loop(acceptor);
    event_loop.setAllCallback(onConnection, onMessage, onClose);
    event_loop.loop();
    return 0;
}