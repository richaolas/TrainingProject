#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    void setUserName(const QString& username) {_username = username;}

    void onRecvMessage(const QString & msg);

signals:
    void updateTextEdit(const QString & msg);


private slots:
    void on_pushButton_clicked();
    void onUpdateTextEdit(const QString & msg);

private:
    Ui::Dialog *ui;
    QString _username;
};

#endif // DIALOG_H
