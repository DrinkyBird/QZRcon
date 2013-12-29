#include "banwindow.h"
#include "rconwindow.h"
#include "ui_banwindow.h"

/* Current time */
QString currenttime;

banwindow::banwindow(RconWindow *rconwindow) :
    QWidget(0),
    ui(new Ui::banwindow),
    rconwindow(rconwindow)
{
    ui->setupUi(this);
}

banwindow::~banwindow()
{
    delete ui;
}

void banwindow::on_cancel_clicked()
{
    this->deleteLater();
}

void banwindow::on_ban_clicked()
{
    rconwindow->okayban(ui->reason->text(),ui->timevalue->text() + ui->timetype->currentText());
    this->deleteLater();
}

/* Ban by enter */
void banwindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter: // Numpad enter
    case Qt::Key_Return: // Main enter
        ui->ban->click();
        break;
    }
}
