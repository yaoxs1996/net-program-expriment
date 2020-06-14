#include "commen.h"

int main()
{
    try
    {
        TCPClient client(8001, "127.0.0.1");
        char buf[BUFSIZ];
        while (fgets(buf, sizeof(buf), stdin) != NULL)
        {
            write(client.getfd(), buf, strlen(buf));
            memset(buf, 0, sizeof(buf));
            read(client.getfd(), buf, sizeof(buf));
            cout << buf;
            memset(buf, 0, sizeof(buf));
        }
    }
    catch (const SocketException &e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}