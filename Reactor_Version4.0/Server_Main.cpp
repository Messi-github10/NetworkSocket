#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "Threadpool.hpp"
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

Threadpool threadpool(4, 10);

class MyTask{
public:
    MyTask(const string & message, TcpConnectionPtr conn)
    :_message(message)
    ,_conn(conn)
    {}

    void process(){
        cout << "MyTask is running..." << endl;

        // 解码 -> 计算 -> 编码
        string response = _message;
        _conn->sendInLoop(response);
    }

private:
    string _message;
    TcpConnectionPtr _conn;
};

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

    // MyTask
    MyTask mytask(message, tcp_connection_ptr);
    threadpool.addTask(std::bind(&MyTask::process, mytask));
}

void onClose(TcpConnectionPtr tcp_connection_ptr){
    cout << tcp_connection_ptr->toString() << "has closed." << endl;
}

int main(){
    threadpool.startThreadPool();
    TcpServer server(8000);
    server.setAllCallBacks(onConnection, onMessage, onClose);
    server.start();
    return 0;
}
