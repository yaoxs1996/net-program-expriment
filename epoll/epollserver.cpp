#include "commen.h"

int main()
{
    signal(SIGPIPE, SIG_IGN);
    char buf[BUFSIZ];
    int clientCount = 0;
    try
    {
        TCPServer server(8001);
        int listenfd = server.getfd();
        Epoll epoll;
        // 将监听套接字注册到epoll
        epoll.addfd(server.getfd(), EPOLLIN, true);
        while (true)
        {
            int nReady = epoll.wait();
            for (int i = 0; i < nReady; ++i)
                // 如果是监听套接字发生了可读事件
                if (epoll.getEventOccurfd(i) == listenfd)
                {
                    int connectfd = accept(listenfd, NULL, NULL);
                    if (connectfd == -1)
                        err_exit("accept error");
                    cout << "accept success..." << endl;
                    cout << "clientCount = " << ++ clientCount << endl;
                    setUnBlock(connectfd, true);
                    epoll.addfd(connectfd, EPOLLIN, true);
                }
                else if (epoll.getEvents(i) & EPOLLIN)
                {
                    TCPClient *client = new TCPClient(epoll.getEventOccurfd(i));
                    memset(buf, 0, sizeof(buf));
                    if (client->read(buf, sizeof(buf)) == 0)
                    {
                        cerr << "client connect closed..." << endl;
                        // 将该套接字从epoll中移除
                        epoll.delfd(client->getfd());
                        delete client;
                        continue;
                    }
                    cout << buf;
                    client->write(buf);
                }
        }
    }
    catch (const SocketException &e)
    {
        cerr << e.what() << endl;
        err_exit("TCPServer error");
    }
    catch (const EpollException &e)
    {
        cerr << e.what() << endl;
        err_exit("Epoll error");
    }
}