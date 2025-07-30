#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include <iostream>
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

        string messages = conn.receiveWithPrefix();

        cout << "Server receive message is [" << messages << "]." << endl;

        conn.sendWithPrefix(messages);
    }
    return 0;
}