#include "cvarwatchwindow.h"
#include "ui_cvarwatchwindow.h"

QStringList watching = QStringList();

CvarWatchWindow::CvarWatchWindow(RconWindow *rconwindow) :
    QWidget(0),
    ui(new Ui::CvarWatchWindow),
    rconwindow(rconwindow)
{
    ui->setupUi(this);
    connect(rconwindow->rcon, SIGNAL(alreadyWatchingCvar(QString)), this, SLOT(onCvarAlreadyWatched(QString)));
}

CvarWatchWindow::~CvarWatchWindow()
{
    delete ui;
}

void CvarWatchWindow::on_watchCvarButton_clicked()
{
    QString toWatch = ui->lineEdit->text();
    qDebug() << QString("Requesting for %1 to be watched.").arg(toWatch);
    rconwindow->rcon->watchCvar(toWatch);
}

void CvarWatchWindow::onCvarAlreadyWatched(QString name)
{
    qWarning() << "Cvar " << name.toStdString().c_str() << " is already being watched";
}
