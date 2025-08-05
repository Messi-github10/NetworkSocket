#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "Threadpool.hpp"
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

class MyTask
{
public:
    MyTask(const string &message, TcpConnectionPtr conn)
        : _message(message), _conn(conn)
    {
    }

    void process()
    {
        cout << "MyTask::process() is running..." << endl;

        // 解码 -> 计算 -> 编码
        string response = _message;
        _conn->sendInLoop(response);
    }

private:
    string _message;
    TcpConnectionPtr _conn;
};

// 整个服务器的业务入口
class EchoServer
{
public:
    // 初始化服务器的端口、IP地址、线程池中的线程数量、任务队列长度
    EchoServer(unsigned short port, const string &ip, int threadNum, int queSize)
        : _threadpool(threadNum, queSize), _tcp_server(port, ip)
    {
        printf("EchoServer...\n");

        // 将EchoServer的成员函数绑定为回调函数
        using namespace std::placeholders;
        _tcp_server.setAllCallBacks(
            std::bind(&EchoServer::onConnection, this, _1), // 连接建立回调
            std::bind(&EchoServer::onMessage, this, _1),    // 消息到达回调
            std::bind(&EchoServer::onClose, this, _1));     // 连接关闭回调
    }

    // 关闭服务器
    ~EchoServer()
    {
        _threadpool.stopThreadPool();
    }

    // 开启服务器
    void start()
    {
        _threadpool.startThreadPool();
        _tcp_server.start();
    }

private:
    // 连接建立实际操作的函数
    void onConnection(TcpConnectionPtr tcp_connection_ptr)
    {
        cout << tcp_connection_ptr->toString() << "has connected." << endl;
    }

    // ------------------------- 业务扩展 -----------------------------------
    // 消息到达实际操作函数
    void onMessage(TcpConnectionPtr tcp_connection_ptr)
    {
        // 获取消息
        string message = tcp_connection_ptr->receive();
        cout << "message size is [" << message.size() << "]." << endl;
        cout << "message is [" << message << "]." << endl;

        // 将处理逻辑（process方法）提交给线程池处理
        MyTask mytask(message, tcp_connection_ptr);
        _threadpool.addTask(std::bind(&MyTask::process, mytask));
    }
    // --------------------------------------------------------------------

    // 连接关闭实际操作函数
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
