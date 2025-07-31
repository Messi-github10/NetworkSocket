#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{

    Acceptor acceptor(8000);
    acceptor.ready();
    printf("Server start listening...\n");

    while (1)
    {
        TcpConnection conn(acceptor.accept());
        cout << conn.toString() << " has connected." << endl;
        string msg = conn.receive();
        if (msg.empty())
        {
            // 情况1：连接断开（正常关闭或错误）
            cout << "Connection closed by peer." << endl;
            break;
        }
        else if (msg == "[NO_DATA]")
        {
            // 情况2：非阻塞无数据，等待并重试
            usleep(100000); // 避免CPU空转（100ms）
            continue;
        }
        else
        {
            // 情况3：正常数据
            cout << "Received: " << msg << endl;
            conn.send(msg);
        }
    }
    return 0;
}