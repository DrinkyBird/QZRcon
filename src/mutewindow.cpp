#include "mutewindow.h"
#include "rconwindow.h"
#include "ui_mutewindow.h"

mutewindow::mutewindow(RconWindow *rconwindow) :
    QWidget(0),
    ui(new Ui::mutewindow),
    rconwindow(rconwindow)
{
    ui->setupUi(this);
}

mutewindow::~mutewindow()
{
    delete ui;
}

void mutewindow::on_cancel_clicked()
{
    this->deleteLater();
}

void mutewindow::on_mute_clicked()
{
    rconwindow->okaymute(ui->time->text());
    this->deleteLater();
}

/* Kick by enter */
void mutewindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter: // Numpad enter
    case Qt::Key_Return: // Main enter
        ui->mute->click();
        break;
    }
}
