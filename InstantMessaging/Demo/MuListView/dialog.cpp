#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>

#include "Widget.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setAttribute (Qt::WA_DeleteOnClose);

    connect(this, &Dialog::updateTextEdit, this, &Dialog::onUpdateTextEdit);
}

Dialog::~Dialog()
{
    Widget* w = (Widget*)(this->parent());
    auto iter = w->_chatDialogs.find (_username.toStdString()) ; // Returns end iterator if key not found
    if(iter != w->_chatDialogs.end())
        iter = w->_chatDialogs.erase (iter) ;// Returns iterator for element after "May"

    delete ui;

}

void Dialog::on_pushButton_clicked()
{
    Widget* w = (Widget*)(this->parent());
    QString s = this->ui->textEdit->toPlainText();
    qDebug() << s;

    string name = _username.toStdString();
    string message = s.toStdString();

    Json::Value root;
    root["sender"] = MBytesToUTF8(std::string("user1"));
    root["recvicer"] = MBytesToUTF8(name);
    root["message"] = MBytesToUTF8(message);

    w->_client->sendMsg(MSG_MESSAGE, root.toStyledString());


    //w->_client->sendMsg(MSG_MESSAGE, "hello");
    //ui->
}

void Dialog::onUpdateTextEdit(const QString &msg)
{
    this->ui->textEdit->setText(msg);
}

void Dialog::onRecvMessage(const QString &msg)
{
    emit updateTextEdit(msg);
}
