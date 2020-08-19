#include "userclient.h"

#include "Widget.h"

int UserClient::handle_message(const string &msg)
{
    Json::Value root;
    if (parse(msg, root) == 0)
    {
        std::string sender = MBytes(root["sender"].asString());
        std::string recvicer = MBytes(root["recvicer"].asString());
        std::string message = MBytes(root["message"].asString());
        //std::cout << sender << "->" << recvicer << ":" << message << std::endl;

        // just transfer
        cout << "[From]    " << sender << "\n"
             << "[Message] " << message << endl;
        cout << "--------------------------------------------------------\n";

        this->_mainWindow->onRecvChatMessage(QString::fromStdString(sender), QString::fromStdString(message));
    }
    return 0;
}
