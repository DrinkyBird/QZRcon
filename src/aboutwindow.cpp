#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include "version.h"

aboutwindow::aboutwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutwindow)
{
    ui->setupUi(this);
    QString text = ui->version->text();
    text.replace("Version", "Version: " + QString(VERSION));
    ui->version->setText(text);
}

aboutwindow::~aboutwindow()
{
    delete ui;
}

void aboutwindow::on_pushButton_clicked()
{
    this->deleteLater();
}
