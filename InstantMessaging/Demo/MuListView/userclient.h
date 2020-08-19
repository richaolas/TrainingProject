#ifndef USERCLIENT_H
#define USERCLIENT_H


#define  _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>

#include <iostream>
#include <thread>
#include <unordered_map>
using namespace std;

#include "SocketUtility.h"
#include "CharacterCodes.h"
#include "JsonUtility.h"


#define DEFAULT_PORT 8000

class Widget;

class UserClient : public Client
{
public:
    UserClient(SOCKET socket) : Client(socket) {}
    // msg handler
    virtual int handle_message(const std::string& msg);

    virtual int handle_login_message(const std::string& msg)
    {
        Json::Value root;
        if (parse(msg, root) == 0)
        {
            std::string sender = MBytes(root["sender"].asString());
            //std::string recvicer = root["recvicer"].asString();
            //std::string message = root["message"].asString();
            std::cout << sender << " log in!" << endl;
            cout << "--------------------------------------------------------\n";
        }
        return 0;
    }

    Widget * _mainWindow;
};


#endif // USERCLIENT_H
