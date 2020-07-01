#pragma execution_character_set("utf-8")

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

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

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

    QMessageBox msg;
    msg.setWindowTitle("Info");
    msg.setText(itemData.singer);
    msg.exec();
}
