#include "kickwindow.h"
#include "rconwindow.h"
#include "ui_kickwindow.h"

kickwindow::kickwindow(RconWindow *rconwindow):
    QWidget(0),
    ui(new Ui::kickwindow),
    rconwindow(rconwindow)
{
    ui->setupUi(this);
}

kickwindow::~kickwindow()
{
    delete ui;
}

void kickwindow::on_kick_clicked()
{
    rconwindow->okaykick(ui->reason->text());
    this->deleteLater();
}

void kickwindow::on_cancel_clicked()
{
    this->deleteLater();
}

/* Kick by enter */
void kickwindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter: // Numpad enter
    case Qt::Key_Return: // Main enter
        ui->kick->click();
        break;
    }
}
