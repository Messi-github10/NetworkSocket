#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "Threadpool.hpp"
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

Threadpool threadpool(4, 10);

class MyTask
{
public:
    MyTask(const string &message, TcpConnectionPtr conn)
        : _message(message), _conn(conn)
    {
    }

    void process()
    {
        cout << "MyTask is running..." << endl;

        // 解码 -> 计算 -> 编码
        string response = _message;
        _conn->sendInLoop(response);
    }

private:
    string _message;
    TcpConnectionPtr _conn;
};

class EchoServer
{
public:
    EchoServer(unsigned short port, const string &ip, int threadNum, int queSize)
    :_threadpool(threadNum, queSize)
    ,_tcp_server(port, ip)
    {
        printf("EchoServer...\n");
        using namespace std::placeholders;
        _tcp_server.setAllCallBacks(
            std::bind(&EchoServer::onConnection, this, _1),
            std::bind(&EchoServer::onMessage, this, _1),
            std::bind(&EchoServer::onClose, this, _1));
    }

    void start(){
        _threadpool.startThreadPool();
        _tcp_server.start();
    }

private:
    void onConnection(TcpConnectionPtr tcp_connection_ptr)
    {
        // 打印五元组信息
        cout << tcp_connection_ptr->toString() << "has connected." << endl;
    }

    // ---------------- 数据处理 -------------------
    // 可扩展
    void onMessage(TcpConnectionPtr tcp_connection_ptr)
    {
        // 获取消息
        string message = tcp_connection_ptr->receive();
        cout << "message size is [" << message.size() << "]." << endl;
        cout << "message is [" << message << "]." << endl;

        // MyTask
        MyTask mytask(message, tcp_connection_ptr);
        threadpool.addTask(std::bind(&MyTask::process, mytask));
    }

    void onClose(TcpConnectionPtr tcp_connection_ptr)
    {
        cout << tcp_connection_ptr->toString() << "has closed." << endl;
    }

private:
    Threadpool _threadpool;
    TcpServer _tcp_server;
};

int main()
{
    EchoServer server(8000, "0.0.0.0", 4, 10);
    server.start();
    return 0;
}
