#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QModelIndex>

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
};

#endif // WIDGET_H
