#include <iostream>

#include "Socket.h"

using namespace std;

int main()
{
    char buf[BUFSIZ];

    try
    {
        int listenfd;
        TCPClient client;
        TCPServer server(8001);
        listenfd = server.getfd();
        while(true)
        {
            TCPClient *client = new TCPClient(server.accept());
            memset(buf, 0, sizeof(buf));
            cout << buf;
            client->write(buf);
        }
    }
    catch(const SocketException &e)
    {
        std::cerr << e.what() << '\n';
        perror("TCPServer error");
        exit(EXIT_FAILURE);
    }
}