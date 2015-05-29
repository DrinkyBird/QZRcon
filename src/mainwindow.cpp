#include "mainwindow.h"
#include "rconwindow.h"
#include "aboutwindow.h"
#include "ui_mainwindow.h"
#include "rcon.h"
#include "version.h"

/* Global settings object */
QSettings *settings = NULL;

/* Global config object */
QSettings *inputconfig = NULL;
QSettings *outputconfig = NULL;

/* Global IDE config object */
QSettings *idesettings = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "QZRcon" << QString(VERSION).toStdString().c_str() << "started. Hello, world! :)";

    ui->setupUi(this);
    setStatus(tr("Ready"));

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    rcon = new Rcon(this);
    connect(rcon, SIGNAL(error(int)), this, SLOT(onError(int)));
    connect(rcon, SIGNAL(stateChanged(int)), this, SLOT(onStateChange(int)));

    loadConfig();
    loadIDEConfig();

    /* Set password style to line */
    ui->linePassword->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::lockGUI(bool lock)
{
    ui->lineAddress->setDisabled(lock);
    ui->linePassword->setDisabled(lock);
    ui->spinPort->setDisabled(lock);

    if (!lock)
        ui->savepassword->setEnabled(true);
}

void MainWindow::setStatus(QString status)
{
    ui->statusBar->showMessage(status);
}

void MainWindow::showError(QString message)
{
    ui->statusBar->showMessage(message);
    QMessageBox::warning(this, "QZRcon error", message);

    /* Re-enable connect button */
    ui->buttonConnect->setText(tr("Connect"));
    ui->buttonConnect->setEnabled(true);
}

/* Connect by enter */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter: // Numpad enter
    case Qt::Key_Return: // Main enter
        ui->buttonConnect->click();
        break;
    case Qt::Key_F10:
        rcon->forceloggedin();
        break;
    }
}

/* Begin host lookup */
void MainWindow::on_buttonConnect_clicked()
{
    ui->buttonConnect->setText(tr("Cancel"));
    saveConfig();

    /* If state == connecting then force disconnect and return hud to default */
    if (rcon->checkCancelEvent())
    {
    ui->buttonConnect->setText(tr("Connect"));
    }
    else
    {
    setStatus(tr("Looking up host address..."));
    lockGUI(true);
    QHostInfo::lookupHost(ui->lineAddress->text(), this, SLOT(beginConnecting(QHostInfo)));
    }
}

/* Begin actually connecting to the server */
void MainWindow::beginConnecting(QHostInfo hostinfo)
{
    /* Invalid address */
    if (hostinfo.error() || hostinfo.addresses().empty())
    {
        showError(hostinfo.errorString());
        lockGUI(false);
        return;
    }

    QHostAddress address = hostinfo.addresses().first();
    rcon->connectToHost(address, ui->spinPort->value(), ui->linePassword->text());
}

/* Handle Rcon errors */
void MainWindow::onError(int e)
{
    switch (e)
    {
    case Rcon::Banned:
        showError(tr("You're banned on this server"));
        break;

    case Rcon::InvalidVersion:
        showError(tr("Version mismatch between server and QZRcon"));
        break;

    case Rcon::InvalidPassword:
        showError(tr("Wrong password"));
        break;

    case Rcon::TimedOut:
        showError(tr("Timed out"));
        break;
    }

    lockGUI(false);
}

/* Display status message or switch to rcon window */
void MainWindow::onStateChange(int state)
{
    switch (state)
    {
    case Rcon::Connecting:
        setStatus(tr("Connecting..."));
        break;

    case Rcon::SendingPassword:
        ui->savepassword->setDisabled(true);
        setStatus(tr("Sending password... Press F10 to force!"));
        break;

    case Rcon::Canceled:
        lockGUI(false);
        setStatus(tr("Canceled"));
        break;

    case Rcon::LoggedIn: // Switch to rcon window.
        RconWindow *w = new RconWindow(0, rcon);
        w->show();
        this->deleteLater();
        break;
    }
}

/* Settings file */
void MainWindow::loadConfig()
{
    if (settings == NULL)
        settings = new QSettings("qzrcon.cfg", QSettings::IniFormat);

    settings->beginGroup("Server");
    ui->lineAddress->setText(settings->value("address", "").toString());
    ui->spinPort->setValue(settings->value("port", "10666").toInt());
    ui->linePassword->setText(settings->value("password", "").toString());
    settings->endGroup();
}

/* For Internet Doom Explorer Support */
void MainWindow::loadIDEConfig()
{
    QString filename = "settings.ini";

    QFile ideconfigfile(filename);


    if (ideconfigfile.exists())
    {
        if (idesettings == NULL)
            idesettings = new QSettings(filename, QSettings::IniFormat);

        QStringList address;

        qDebug() << "Loaded Internet Doom Explorer" << filename << "config file.";
        idesettings->beginGroup("Settings");
        address = idesettings->value("LastServer", "").toString().split(':');

        if (address.length() == 2)
        {
            ui->lineAddress->setText(address[0]);
            ui->spinPort->setValue(QString(address[1]).toInt());
        }

        idesettings->endGroup();
    }
}

void MainWindow::saveConfig()
{
    settings->beginGroup("Server");
    settings->setValue("address", ui->lineAddress->text());
    settings->setValue("port", ui->spinPort->value());
    if (ui->savepassword->isChecked()) settings->setValue("password", ui->linePassword->text());
    settings->endGroup();

    settings->sync();

}

/* Show about window */
void MainWindow::on_actionAbout_triggered()
{
    aboutwindow *w = new aboutwindow(0);
    w->show();
}

/* Configuration file */
void MainWindow::on_actionLoad_config_triggered()
{
    QString filename = QFileDialog::getOpenFileName
            (this, tr("Open config"),"",tr("Config file (*.cfg)"));

    if (filename != NULL)
    {
    if(inputconfig == NULL)
        inputconfig = new QSettings(filename, QSettings::IniFormat);

    inputconfig->beginGroup("Server");
    ui->lineAddress->setText(inputconfig->value("address", "").toString());
    ui->spinPort->setValue(inputconfig->value("port", "10666").toInt());
    ui->linePassword->setText(inputconfig->value("password").toString());
    inputconfig->endGroup();
    }
}

void MainWindow::on_actionSave_config_triggered()
{
    QString filename = QFileDialog::getSaveFileName
            (this, tr("Save config"),"",tr("Config file (*.cfg)"));

    if (filename != NULL)
    {
    if (outputconfig == NULL)
        outputconfig = new QSettings(filename, QSettings::IniFormat);

    outputconfig->beginGroup("Server");
    outputconfig->setValue("address", ui->lineAddress->text());
    outputconfig->setValue("port", ui->spinPort->value());
    if (ui->savepassword->isChecked())
        outputconfig->setValue("password", ui->linePassword->text());
    outputconfig->endGroup();

    outputconfig->sync();
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}
