#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>

namespace Ui {
class aboutwindow;
}

class aboutwindow : public QDialog
{
    Q_OBJECT

public:
    explicit aboutwindow(QWidget *parent = 0);
    ~aboutwindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::aboutwindow *ui;
};

#endif // ABOUTWINDOW_H
