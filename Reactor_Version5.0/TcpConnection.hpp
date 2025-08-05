#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"
#include <memory>
#include <string>
#include <functional>
using std::enable_shared_from_this;
using std::shared_ptr;
using std::string;

/*
TcpConnection类：
该类的创建是由Acceptor类的对象调用accept函数得来的，也就是表明三次握手建立成功之后的结果。
可以通过该连接的对象进行收发数据，发送数据的时候封装函数send，接受数据的时候封装函数receive。
*/

class EventLoop;    // 前向声明，防止循环引用
class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>;     // TcpConnection类型的智能指针
using TcpConnectionCallBack = std::function<void(TcpConnectionPtr)>;    // 设置回调函数的类型

class TcpConnection
:public Noncopyable
,public enable_shared_from_this<TcpConnection>  // 防止智能指针的误用（多个不同的智能指针托管同一片区域）
{
public:
    TcpConnection(int, EventLoop *);
    ~TcpConnection();   // 关闭连接 -> 关闭服务器写端
    string receive();   // 接收数据
    void send(const string &);  // 发送数据

    string toString() const;    // 获取五元组信息
    void shutdown();            // 关闭写端
    void setAllCallBacks(const TcpConnectionCallBack &, const TcpConnectionCallBack &, const TcpConnectionCallBack &);  // 设置回调函数
    
    // 对回调函数进行二次封装，调用者无需关心底层细节
    void handleConnectionCallBack();
    void handleMessageCallBack();
    void handleCloseCallBack();

    bool isClosed() const;  // 连接是否关闭

    void sendInLoop(const string &message); // 将数据发送操作转移到 EventLoop 的事件循环中执行（因此该类需要记录所属的事件循环）

private:

    InetAddress getLocalAddr(int);  // 获取服务器本地的IP地址
    InetAddress getPeerAddr(int);   // 获取对端的IP地址

private:
    Socket _sock;
    SocketIO _socketIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutDownWrite;
    EventLoop *_loop;   // 所属的事件循环（用于跨线程任务调度）

    TcpConnectionCallBack _onConnection;
    TcpConnectionCallBack _onMessage;
    TcpConnectionCallBack _onClose;
};

#endif