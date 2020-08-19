#pragma execution_character_set("utf-8")

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
#include "dialog.h"


#include "userclient.h"

#include <QStandardItemModel>
#include <QObject>
#include <QMessageBox>
#include "Widget.h"
#include "ui_Widget.h"
#include "MuListItemData.h"
#include "MuItemDelegate.h"


const QStringList icons = {
    ":/images/HotDog.jpg", ":/images/li.jpg", ":/images/logo.jpg",
    ":/images/PACT.jpg", ":/images/yang.jpg", ":/images/zhang.jpg",
};
const QStringList singers = {
    "MC-Hotdog", "李荣浩", "Author", "PACT", "杨千嬅", "张震岳",
};

bool Widget::connectServer()
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
    SOCKET sockClt = socket(AF_INET, SOCK_STREAM, 0);
    if (sockClt == INVALID_SOCKET) {
        cout << "socket() fail:" << WSAGetLastError() << endl;
        return -2;
    }
    //(3)IP
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrSrv.sin_port = htons(DEFAULT_PORT);

    //(5)connect
    err = ::connect(sockClt, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    if (err == INVALID_SOCKET)
    {
        cout << "connect() fail" << WSAGetLastError() << endl;
        return -1;
    }

    char sendBuf[1024], hostname[100];
    if (gethostname(hostname, 100) != 0)    //get host name
        strcpy(hostname, "None");
    strcpy(sendBuf, hostname);
    strcat(sendBuf, "have connected to you!");

    cout << "1 " << sockClt << endl;
    //UserClient c(sockClt);
    //c.runHandler();
    //cout << "1" << c.

    //std::thread t(recvthread, &c);
    _client = new UserClient(sockClt);
    _client->_mainWindow = this;
    _client->runHandler();

    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;
    for (int i = 0; i < 10; i++)
    {
        item["key"] = i;
        arrayObj.append(item);
    }

    cout << "please input your name" << endl;
    string username = "user1";
    //cin >> username;
    root["user"] = username;
    //root["key2"] = "value2";
    //root["array"] = arrayObj;
    //root.toStyledString();
    std::string out = root.toStyledString();
    //std::cout << out << std::endl;

    strcpy(sendBuf, out.c_str());
    _client->sendMsg(MSG_LOGIN, root.toStyledString());

    cout << "input user, and message (intput quit for quit)" << endl;

    return true;
}

void Widget::onRecvChatMessage(const QString &from, const QString &chatMsg)
{
    auto iter = _chatDialogs.find (from.toStdString()) ; // Returns end iterator if key not found
    if(iter != _chatDialogs.end())
    {
        iter->second->onRecvMessage(chatMsg);
    }
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connectServer();

    QStandardItemModel *pModel = new QStandardItemModel();
    for (int i=0; i<icons.size(); ++i) {
        QStandardItem *pItem = new QStandardItem;
        MuItemData itemData;
        itemData.singer = singers.at(i);
        itemData.songsNb = QString::number(i * i) + "首";
        itemData.iconPath = icons.at(i);
        pItem->setData(QVariant::fromValue(itemData), Qt::UserRole+1);
        pModel->appendRow(pItem);
    }

    m_model = pModel;

    MuItemDelegate *pItemDelegate = new MuItemDelegate(this);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView->setItemDelegate(pItemDelegate);
    ui->listView->setModel(m_model);
    //connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(showClick(QModelIndex)));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showClick(QModelIndex)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    for (int i=0;i<m_model->rowCount();i++)
    {
        QStandardItem * item = m_model->item(i);
        QVariant var = item->index().data(Qt::UserRole+1);
        MuItemData itemData = var.value<MuItemData>();
        if (itemData.singer == "李荣浩")
        {
            m_model->removeRow(i);
            break;
        }
    }
}

void Widget::showClick(QModelIndex index)
{
    QString strTemp;
    QVariant var = index.data(Qt::UserRole+1);//.value<MuItemData>.singer();
    MuItemData itemData = var.value<MuItemData>();

    //QMessageBox msg;
    //msg.setWindowTitle("Info");
    //msg.setText(itemData.singer);
    //msg.exec();

    Dialog * d = new Dialog(this);
    d->setUserName(itemData.singer);
    _chatDialogs.insert(std::make_pair(itemData.singer.toStdString(), d));
    d->show();

}
