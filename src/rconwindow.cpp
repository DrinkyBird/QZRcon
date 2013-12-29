#include "mainwindow.h"
#include "rconwindow.h"
#include "aboutwindow.h"
#include "kickwindow.h"
#include "banwindow.h"
#include "mutewindow.h"
#include "ui_rconwindow.h"

/* Static functions */
static QString escapeMessage(QString str); // convertion to command-like string type
extern QSettings *settings; // Global settings object.

/* Current selected player */
QString currentname; // name of player selected in playerlist

/* Data status strings */
QString mapnamedata = "not changed"; // Default

bool enabletime = true; // local time string condition

int admincountdata; // count of admins
int playercountdata; // count of players

QString chatsound; // chat sound file

/* Firstline condiions */
bool firstline = true;
bool firstlinex = true; // for logs

QString servername; // server name
QString logautosavepath; // log path

QString defcolor = "000000"; // default color (black)
/* ----------------------------------------------- */

RconWindow::RconWindow(QWidget *parent, Rcon *rcon) :
    QMainWindow(parent),
    ui(new Ui::RconWindow),
    rcon(rcon)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose); // To send disconnect packet on close.

    rcon->setParent(this);
    connect(ui->buttonSend, SIGNAL(clicked()), this, SLOT(send()));
    connect(rcon, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
    connect(rcon, SIGNAL(mapChanged(QString)), this, SLOT(onMapChanged(QString)));
    connect(rcon, SIGNAL(adminCountChanged(int)), this, SLOT(onAdminCountChanged(int)));
    connect(rcon, SIGNAL(playerCountChanged(int)), this, SLOT(onPlayerCountChanged(int)));
    connect(rcon, SIGNAL(playerListChanged(QList<PlayerInfo>)), this, SLOT(onPlayerListChanged(QList<PlayerInfo>)));
    connect(rcon, SIGNAL(servernamereceived(QString)), this, SLOT(onServerNameReceived(QString)));
    connect(rcon, SIGNAL(serverlog(QString)), this, SLOT(onServerLogReceived(QString)));

    /* Restore variables from config */
    loadConfig();
}

RconWindow::~RconWindow()
{
    /* Save variables to config */
    saveConfig();

    /* Auto save logs */
    saveLog();

    delete settings;
    delete ui;
}

void RconWindow::loadConfig()
{
    settings->beginGroup("User");
    ui->lineNickname->setText(settings->value("nickname", "").toString());
    settings->endGroup();

    settings->beginGroup("Rcon");
    ui->actionEnable_color_tags->setChecked(settings->value("colortags", true).toBool());
    ui->actionRemove_log_messages->setChecked(settings->value("logmessages", true).toBool());
    ui->actionShow_message_time->setChecked(settings->value("messagetime", true).toBool());
    ui->actionRemove_server_log->setChecked(settings->value("noserverlog", false).toBool());
    ui->actionMove_cursor_to_end_on_message->setChecked(settings->value("movecursor", true).toBool());
    settings->endGroup();

    settings->beginGroup("Sound");
    ui->actionPlay_chat_sound->setChecked(settings->value("chatsound", false).toBool());
    chatsound = settings->value("chatsoundpath", QString(QDir::currentPath() + QDir::separator() + "dsradio.wav")).toString();
    settings->endGroup();

    settings->beginGroup("Log");
    ui->actionAuto_save_logs->setChecked(settings->value("autosavelogs", false).toBool());
    ui->actionSave_in_HTML_Document->setChecked(settings->value("autosavelogshtml", false).toBool());
    logautosavepath = settings->value("autosavelogspath", QDir::currentPath()).toString();
    settings->endGroup();

    settings->beginGroup("Style");
    QFont font;
    font.fromString(settings->value("font", QFont("Courier", 8).toString()).toString());
    ui->log->setFont(font);
    settings->endGroup();
}

void RconWindow::saveConfig()
{
    settings->beginGroup("User");
    settings->setValue("nickname", ui->lineNickname->text());
    settings->endGroup();

    /* Check interface and save that stuff */
    on_actionEnable_color_tags_activated();
    on_actionRemove_log_messages_activated();
    on_actionShow_message_time_activated();
    on_actionRemove_server_log_triggered();
    on_actionMove_cursor_to_end_on_message_triggered();
    on_actionPlay_chat_sound_triggered();
    on_actionAuto_save_logs_triggered();
    on_actionSave_in_HTML_Document_triggered();
}

void RconWindow::saveLog()
{
if (ui->actionAuto_save_logs->isChecked())
{
    QString filename;
    QString format;

    if (ui->actionSave_in_HTML_Document->isChecked()) format = ".html"; else format = ".txt";

    filename = logautosavepath + QDir::separator()
            + "autosave_"
            + QDate::currentDate().toString("ddd_MMMM_yyyy")
            + "-"
            + QTime::currentTime().toString("hh_mm_ss_ap")
            + format;

    if (filename != NULL)
    {
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);

            if (ui->actionSave_in_HTML_Document->isChecked()) out << QString(ui->tmplog->toHtml());
            else out << QString(ui->tmplog->toPlainText());

            qDebug() << filename << "log saved!";
        }

        file.close();
    }
}
}



/* Color tags */
// -----------------------------------------------------------------------//
/* In fact white color is gray, but very light - visible on fully-white background */

static const struct colorinfo { char letter; QString color; }
colorcodes[] =
{
    { 'l', defcolor }, // Original (Black)
    { '-', defcolor }, // Default Print (Black)
    { '+', defcolor }, // Default PrintBold (Black)
    { '*', defcolor }, // Default Chat (Black)
    { '!', defcolor }, // Default Team Chat (Black)

    { 'a', "B22222" }, // reddish brick
    { 'b', "D2B48C" }, // tan
    { 'c', "BEBEBE" }, // gray
    { 'd', "00FF00" }, // green
    { 'e', "A52A2A" }, // brown
    { 'f', "FFD700" }, // gold
    { 'g', "FF0000" }, // red
    { 'h', "0000FF" }, // blue
    { 'i', "FFA500" }, // orange
    { 'j', "EBDDE2" }, // white
    { 'k', "FFFF00" }, // yellow
    { 'm', "000000" }, // black
    { 'n', "87CEFA" }, // light blue
    { 'o', "FFDAB9" }, // cream
    { 'p', "698B22" }, // olive
    { 'q', "006400" }, // dark green
    { 'r', "8B0000" }, // dark red
    { 's', "8B2323" }, // dark brown
    { 't', "A020F0" }, // purple
    { 'u', "A9A9A9" }, // dark gray
    { 'v', "00FFFF" }, // cyan
};

/* Color strings */
static const struct xcolorinfo { QString colorstring; QString color; }
colorstrings[] =
{
    { "[reddishbrick]", "B22222" }, // reddish brick
    { "[tan]", "D2B48C" }, // tan
    { "[gray]", "BEBEBE" }, // gray
    { "[green]", "00FF00" }, // green
    { "[brown]", "A52A2A" }, // brown
    { "[gold]", "FF0000" }, // gold
    { "[red]", "FF0000" }, // red
    { "[blue]", "0000FF" }, // blue
    { "[orange]", "FFA500" }, // orange
    { "[white]", "EBDDE2" }, // white
    { "[yellow]", "FFFF00" }, // yellow
    { "[black]", "000000" }, // black
    { "[lightblue]", "87CEFA" }, // light blue
    { "[cream]", "FFDAB9" }, // cream
    { "[olive]", "698B22" }, // olive
    { "[darkgreen]", "006400" }, // dark green
    { "[darkred]", "8B0000" }, // dark red
    { "[darkbrown]", "8B2323" }, // dark brown
    { "[purple]", "A020F0" }, // purple
    { "[darkgray]", "A9A9A9" }, // dark gray
    { "[cyan]", "00FFFF" }, // cyan
};
// -----------------------------------------------------------------------//

QString RconWindow::ProcessColors(QString str)
{
    for (size_t i = 0; i < (sizeof colorstrings / sizeof *colorstrings); ++i)
    {
        const struct xcolorinfo& string = colorstrings[i];
        QString xreplacement;

        if (ui->actionEnable_color_tags->isChecked())
        {
            xreplacement = QString("<font color=#%1>").arg(string.color);
        }

        str.replace(QString("\\c%1").arg(string.colorstring), xreplacement);
        str.replace(QString("\\c%1").arg(QString(string.colorstring).toUpper()), xreplacement);
    }

    /* Replace tags to HTML code */
    for (size_t i = 0; i < (sizeof colorcodes / sizeof *colorcodes); ++i)
    {
        const struct colorinfo& code = colorcodes[i];
        QString replacement;

        if (ui->actionEnable_color_tags->isChecked())
        {
            replacement = QString("<font color=#%1>").arg(code.color);
        }

        str.replace(QString("\\c%1").arg(code.letter), replacement);
        str.replace(QString("\\c%1").arg(QChar(code.letter).toUpper()), replacement);
    }

    return str;
}

/* Receive a server log */
void RconWindow::onServerLogReceived(QString message)
{
    if (!ui->actionRemove_server_log->isChecked())
    {
        enabletime = false;
        printMessage(message);
    }
    enabletime = true;
}

/* Receive a message */
void RconWindow::onMessage(QString message)
{
    /* Lines with this string will be ignored */
    QString ignorem = "(RCON by ";

    /* Current date */
    QDateTime time;
    QString messagetime;

    if (enabletime) time = time.currentDateTime();

    /* Replace damn protocol color codes to normal color codes */
    message.replace(QString(""), QString("\\c"));

    /* For HTML code */
    QString nextline;
    QString nextlinex;

    if (!firstline) nextline = "<br>";
    if (!firstlinex) nextlinex = "<br>";

    message.replace(QString("<"), QString("&lt;"));
    message.replace(QString(">"), QString("&gt;"));
    message.replace(QString("\\n"), nextline);

    /*  Replace invalid colors in log messages */
    if (message.lastIndexOf(ignorem) > 0)
        message.replace("\"", QString("\\c-\""));

    /* Replace strings with color tags */
    message = ProcessColors(message);

    /* Show time */
    if (ui->actionShow_message_time->isChecked())
        messagetime = time.toString("[hh:mm:ss ap]");

    /* Show message */
    if (message.lastIndexOf(ignorem) < 0)
    {
        ui->log->insertHtml(nextline + messagetime + message);
        if(ui->actionMove_cursor_to_end_on_message->isChecked())
            ui->log->moveCursor(QTextCursor::End);
    }

    /* Print log messages? */
    if ((message.lastIndexOf(ignorem) > 0) && (!ui->actionRemove_log_messages->isChecked()))
    {
        ui->log->insertHtml(nextline + messagetime + message);
        if(ui->actionMove_cursor_to_end_on_message->isChecked())
            ui->log->moveCursor(QTextCursor::End);
    }

    /* Save messages to another place for log autosaving */
    if (message.lastIndexOf(ignorem) < 0) ui->tmplog->insertHtml(nextlinex + messagetime + message);
    if ((message.lastIndexOf(ignorem) > 0) && (!ui->actionRemove_log_messages->isChecked())) ui->tmplog->insertHtml(nextlinex + messagetime + message);

    /* Check line is first */
    if ((message.lastIndexOf(ignorem) < 0) || ((message.lastIndexOf(ignorem) > 0) && (!ui->actionRemove_log_messages->isChecked())))
    {
        firstline = false;
        firstlinex = false;
    }

    /* Process chat sound */
    if (((chatsound == NULL) || (chatsound == "")) && (ui->actionPlay_chat_sound->isChecked()))
        qDebug() << "Couldn't open sound file: chat sound path is not set!";
    else if((!QSound::isAvailable()) && (ui->actionPlay_chat_sound->isChecked()))
        qDebug() << chatsound << "Couldn't open sound device!";
    else if(ui->actionPlay_chat_sound->isChecked()) QSound::play(chatsound);
}

/* Receive a mapname */
void RconWindow::onMapChanged(QString mapname)
{
    mapnamedata = mapname;
    DrawStatus();
}

/* Receive a admin count */
void RconWindow::onAdminCountChanged(int admincount)
{
    admincountdata = admincount;
    DrawStatus();
}

/* Receive a player count */
void RconWindow::onPlayerCountChanged(int playercount)
{
    playercountdata = playercount;
    DrawStatus();
}

/* Receive a player list*/
void RconWindow::onPlayerListChanged(QList<PlayerInfo> players)
{
    ui->playerlist->clear();
    for (int i = 0; i < players.size(); i++)
        ui->playerlist->addItem(new QListWidgetItem(players[i].name.mid(1)));
}

/* Receive a server name */
void RconWindow::onServerNameReceived(QString name)
{
    QWidget::setWindowTitle("QZrcon Utility Console - " + name.mid(1));
    servername = name.mid(1);
}

/* Draw status on the window */
void RconWindow::DrawStatus()
{
    QString statusstr = QString("Current map: %1 | Other admins: %2 (total %3) | Players online: %4")
            .arg(mapnamedata).arg(admincountdata).arg(admincountdata+1).arg(playercountdata);
    ui->status->setText(statusstr);
    ui->status->setAlignment(Qt::AlignLeft);
}

/* Send command written in the text box */
void RconWindow::send()
{
    /* Set Nickname */
    QString nickname = ui->lineNickname->text();

    /* The first symbols */
    QString cmdstring = "/";
    QString chatstring = ":";
    QString mestring = "me";

    QString command = ui->lineCommand->text();
    if (command.isEmpty()) return;

    /* Send as command is first symbol is / */
    if (ui->checkChat->isChecked())
    {
        if (command.left(3) == cmdstring + mestring)
        {
            command = command.mid(3);
            if (!nickname.isEmpty()) command = nickname + "\\c-" + command;
                command = "say " + cmdstring + mestring + escapeMessage(command);
        }
        else if (command.left(1) == cmdstring) command = command.mid(1); else
        {
            if (!nickname.isEmpty()) command = nickname + "\\c-: " + command;
                command = "say " + escapeMessage(command);
        }
    }

    /* Send as chat message is first symbol is : */
    if (!ui->checkChat->isChecked())
    {
        if (command.left(4) == chatstring + cmdstring + mestring)
        {
            command = command.mid(4);
            if (!nickname.isEmpty()) command = nickname + "\\c-" + command;
                command = "say " + cmdstring + mestring + escapeMessage(command);
        } else
        if (command.left(1) == chatstring)
        {
            command = command.mid(1);
            if (!nickname.isEmpty()) command = nickname + "\\c-: " + command;
                command = "say " + escapeMessage(command);
        }
    }

    rcon->sendCommand(command);
    ui->lineCommand->clear();
}

/* Ask user when exit */
void RconWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ret;

    ret = QMessageBox::question( this,  QApplication::applicationName(), tr("Do you want to close QZrcon?"),
        QMessageBox::Yes | QMessageBox::No , QMessageBox::No );

    if (ret == QMessageBox::No) event->ignore();
}

/* Send commands by enter key */
void RconWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter: // Numpad enter.
    case Qt::Key_Return: // Main enter.
        send();
        break;
    }
}

/* Show error message */
void RconWindow::showMessage(QString message) { QMessageBox::warning(this, "QZRcon error", message); }

/* Escape chat message string */
static QString escapeMessage(QString str)
{
    QString out;
    out += "\"";

    for (int i = 0; i < str.size(); i++)
    {
        QChar c = str.at(i);
        if (c == '\\') out += "\\";
        else if (c == '\"') out += "\\\"";
        else out += c;
    }

    out += "\"";
    return out;
}

/* Clear all messages in console */
void RconWindow::on_actionClean_triggered()
{
    ui->log->clear();
    firstline = true;
}

/* Show about window */
void RconWindow::on_actionAbout_triggered()
{
    aboutwindow *w = new aboutwindow(0);
    w->show();
}

/* Options */
void RconWindow::on_actionEnable_color_tags_activated()
{
    settings->beginGroup("Rcon");
    if (ui->actionEnable_color_tags->isChecked()) settings->setValue("colortags",true);
    else settings->setValue("colortags",false);
    settings->endGroup();
}

void RconWindow::on_actionRemove_log_messages_activated()
{
    settings->beginGroup("Rcon");
    if (ui->actionRemove_log_messages->isChecked()) settings->setValue("logmessages",true);
    else settings->setValue("logmessages",false);
    settings->endGroup();
}

void RconWindow::on_actionShow_message_time_activated()
{
    settings->beginGroup("Rcon");
    if (ui->actionShow_message_time->isChecked()) settings->setValue("messagetime",true);
    else settings->setValue("messagetime",false);
    settings->endGroup();
}

void RconWindow::on_actionRemove_server_log_triggered()
{
    settings->beginGroup("Rcon");
    if (ui->actionRemove_server_log->isChecked()) settings->setValue("noserverlog",true);
    else settings->setValue("noserverlog",false);
    settings->endGroup();
}

void RconWindow::on_actionPlay_chat_sound_triggered()
{
    settings->beginGroup("Sound");
    if (ui->actionPlay_chat_sound->isChecked()) settings->setValue("chatsound",true);
    else settings->setValue("chatsound",false);
    settings->endGroup();
}

void RconWindow::on_actionMove_cursor_to_end_on_message_triggered()
{
    settings->beginGroup("Rcon");
    if (ui->actionMove_cursor_to_end_on_message) settings->setValue("movecursor",true);
    else settings->setValue("movecursor",false);
    settings->endGroup();
}

void RconWindow::on_actionAuto_save_logs_triggered()
{
    settings->beginGroup("Log");
    if (ui->actionAuto_save_logs->isChecked()) settings->setValue("autosavelogs",true);
    else settings->setValue("autosavelogs",false);
    settings->endGroup();
}

void RconWindow::on_actionSave_in_HTML_Document_triggered()
{
    settings->beginGroup("Log");
    if (ui->actionSave_in_HTML_Document->isChecked()) settings->setValue("autosavelogshtml",true);
    else settings->setValue("autosavelogshtml",false);
    settings->endGroup();
}

/* Copy player name from playerlist to command line */
void RconWindow::on_playerlist_itemDoubleClicked(QListWidgetItem *item)
{
    ui->lineCommand->setText(ui->lineCommand->text() + item->text());
}

/* Set player name on click */
void RconWindow::on_playerlist_itemPressed(QListWidgetItem *item)
{
    currentname = item->text();
}

/* Actions */
QString playerlistwarn = "You must select player in playerlist!";

void RconWindow::on_actionKick_player_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    rcon->sendCommand("kick " + escapeMessage(currentname));
    currentname = "";
    } else
        showMessage(playerlistwarn);
}


void RconWindow::on_actionForce_spectate_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    rcon->sendCommand("kickfromgame " + escapeMessage(currentname));
    currentname = "";
    } else
        showMessage(playerlistwarn);
}

void RconWindow::on_actionKick_with_reason_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    kickwindow *w = new kickwindow(this);
    w->show();
    } else
        showMessage(playerlistwarn);
}

void RconWindow::okaykick(QString reason)
{
    rcon->sendCommand("kick " + escapeMessage(currentname) + " " + escapeMessage(reason));
    currentname = "";
}

void RconWindow::on_actionBan_player_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    banwindow *w = new banwindow(this);
    w->show();
    } else
        showMessage(playerlistwarn);
}

void RconWindow::okayban(QString reason, QString time)
{
    rcon->sendCommand("ban " + escapeMessage(currentname) + " " + time + " " + escapeMessage(reason));
    currentname = "";
}

void RconWindow::on_actionMute_player_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    mutewindow *w = new mutewindow(this);
    w->show();
    } else
        showMessage(playerlistwarn);
}

void RconWindow::okaymute(QString time)
{
    rcon->sendCommand("ignore " + escapeMessage(currentname) + " " + time);
    currentname = "";
}

void RconWindow::on_actionUnmute_player_triggered()
{
    ui->playerlist->selectionModel()->clear();

    if (currentname != "")
    {
    rcon->sendCommand("unignore " + escapeMessage(currentname));
    currentname = "";
    } else showMessage(playerlistwarn);
}

/* Save console to log file */

/* As plain text */
void RconWindow::on_actionSave_log_triggered()
{
    QString filename = QFileDialog::getSaveFileName
            (this, tr("Save log"),"",tr("Log file (*.txt)"));

    if (filename != NULL)
    {
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        QTextStream out(&file);
        out << QString(ui->log->toPlainText());
        qDebug() << filename << "log saved!";
        }
        file.close();
    }
}

/* As HTML */
void RconWindow::on_actionSave_log_html_triggered()
{
    QString filename = QFileDialog::getSaveFileName
            (this, tr("Save log"),"",tr("HTML Document (*.html)"));

    if (filename != NULL)
    {
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        QTextStream out(&file);
        out << QString(ui->log->toHtml());
        qDebug() << filename << "log saved!";
        }
        file.close();
    }
}

/* Import new chat sound */
void RconWindow::on_actionImport_chat_sound_triggered()
{
    QString filename = QFileDialog::getOpenFileName
            (this, tr("Open chat sound"),"",tr("Wave file (*.wav)"));

    if (filename != NULL)
    {
        settings->beginGroup("Sound");
        settings->setValue("chatsoundpath", filename);
        settings->endGroup();
        chatsound = filename;
    }
}

/* Do reconnect*/
void RconWindow::on_actionReconnect_triggered() { rcon->reconnect(); }

/* Choose autosave path */
void RconWindow::on_actionChoose_autosave_path_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this);

    if (dir != NULL)
    {
        settings->beginGroup("Log");
        settings->setValue("autosavelogspath", dir);
        settings->endGroup();
        logautosavepath = dir;
    }
}

/* Set Custom Font */
void RconWindow::on_actionSet_Font_triggered()
{
    bool ok;

    QFont font = QFontDialog::getFont(&ok, ui->log->font(), this);

    if (ok)
    {
        settings->beginGroup("Style");
        ui->log->setFont(font);
        settings->setValue("font", font.toString());
        settings->endGroup();
    }
}

/* Nickname color preview */
void RconWindow::on_lineNickname_textChanged(const QString &arg1)
{
    ui->colorednickname->setText(ProcessColors(arg1));
    ui->colorednickname->setAlignment(Qt::AlignRight);
}

void RconWindow::on_playerlist_itemClicked(QListWidgetItem *item)
{

}

/* Funny close event */
void RconWindow::on_actionQuit_triggered()
{
    RconWindow::close();
}
