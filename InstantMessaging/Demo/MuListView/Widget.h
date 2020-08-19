#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QModelIndex>

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


#include "userclient.h"

#include <unordered_map>
class Dialog;

namespace Ui {
class Widget;
}

class QStandardItemModel;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void showClick(QModelIndex index);


private:
    Ui::Widget *ui;

    QStandardItemModel *m_model;

public: //socket
    bool connectServer();
    UserClient * _client;
    std::unordered_map<std::string, Dialog*> _chatDialogs;
    void onRecvChatMessage(const QString& from, const QString& chatMsg);
};

#endif // WIDGET_H
