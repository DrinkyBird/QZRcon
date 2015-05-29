#include "cvarwatchwindow.h"
#include "ui_cvarwatchwindow.h"

#include <QMessageBox>

CvarWatchWindow::CvarWatchWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CvarWatchWindow),
    rconwindow(rconwindow),
    Rcon(rcon)
{
    ui->setupUi(this);
    connect(rcon, SIGNAL(watchingCvar(QString, QString)), this, SLOT(onCvarWatched(QString, QString)));
}

CvarWatchWindow::~CvarWatchWindow()
{
    delete ui;
}

void CvarWatchWindow::on_watchButton_clicked()
{

}

void CvarWatchWindow::onCvarWatched(QString cvar, QString value)
{
    QMessageBox infoBox = QMessageBox(QMessageBox::Icon::Information, "QZRcon", "CVar was added to your watch list successfully.");
    infoBox.show();
}
