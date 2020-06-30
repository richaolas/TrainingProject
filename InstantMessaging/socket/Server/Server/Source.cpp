#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>  
#include <iostream>  

#include <string.h>  
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
using namespace std;

#include "SocketUtility.h"
#include "Server.h"

int main()
{
    Server server;
    server.init();
    server.run();

    while (1)
    {
        std::this_thread::sleep_for(1s);
    }

    return 0;
}

int main99(int argc, char* argv[])
{

    WORD    wVersionRequested;
    WSADATA wsaData;
    int     err, iLen;
    wVersionRequested = MAKEWORD(2, 2);//create 16bit data  
//(1)Load WinSock  
    err = WSAStartup(wVersionRequested, &wsaData); //load win socket  
    if (err != 0)
    {
        cout << "Load WinSock Failed!";
        return -1;
    }
    //(2)create socket  
    SOCKET _sockSrv = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockSrv == INVALID_SOCKET) {
        cout << "socket() fail:" << WSAGetLastError() << endl;
        return -2;
    }
    //(3)server IP  
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);//Auto IP, byte sequence change  
    addrSrv.sin_port = htons(DEFAULT_PORT);
    //(4)bind  
    err = bind(_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
    if (err != 0)
    {

        cout << "bind()fail" << WSAGetLastError() << endl;
        return -3;
    }
    //  
//(5)listen  
    err = listen(_sockSrv, 5);
    if (err != 0)
    {

        cout << "listen()fail" << WSAGetLastError() << endl;
        return -4;
    }
    cout << "Server waitting...:" << endl;
    //(6)client ip  
    SOCKADDR_IN addrClt;
    int len = sizeof(SOCKADDR);

    std::vector<std::shared_ptr<Client>> sockets;

    while (1)
    {
        //(7)accept  
        SOCKET sockConn = accept(_sockSrv, (SOCKADDR*)&addrClt, &len);
        char sendBuf[1024], hostname[100];
        if (gethostname(hostname, 100) != 0)    //get host name  
            strcpy(hostname, "None");
        sprintf(sendBuf, "Welecome %s connected to %s(%d)!", inet_ntoa(addrClt.sin_addr), hostname, (int)sockConn);
        printf(sendBuf);
        //sockets.push_back(sockConn);
        //(8)send recv  

        std::shared_ptr < Client> c(new Client(sockConn));        
        //c->runHandler(t);
        sockets.push_back(c);
        c->runHandler();

        
        //std::thread t(msg_handler, c);

#if 1
        //err = send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);

        //char recvBuf[1024] = "\0";
        //iLen = recv(sockConn, recvBuf, 1024, 0);

        //recvBuf[iLen] = '\0';

        
        //(9)close connected sock  
        //closesocket(sockConn);
#endif
    }
    //(10)close server sock  
    closesocket(_sockSrv);
    //(11)clean up winsock  
    WSACleanup();
    return 0;
}