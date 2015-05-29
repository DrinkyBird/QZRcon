#ifndef RCONWINDOW_H
#define RCONWINDOW_H

#include "common.h"
#include "rcon.h"

namespace Ui {
class RconWindow;
}

class RconWindow : public QMainWindow
{
    Q_OBJECT

    struct PlayerInfo
    {
        QString name;
    };

public:
    explicit RconWindow(QWidget *parent, Rcon *rcon);
    ~RconWindow();

    void log(QString string, bool resetCursor = true);
    Rcon *rcon;


public slots:
    void okaykick(QString reason);
    void okayban(QString reason, QString time);
    void okaymute(QString time);

private slots:
    /* Events */
    void onStateChange(int state);
    void onMessage(QString message);
    void onMapChanged(QString mapname);
    void onAdminCountChanged(int admincount);
    void onPlayerCountChanged(int playercount);
    void onPlayerListChanged(QList<PlayerInfo> players);
    void onServerNameReceived(QString name);
    void onServerLogReceived(QString message);
    void onCompletionReceived(QStringList commands, int amount);
    void onTooManyCompletions(int amount);

    QString ProcessColors(QString str);

    void printMessage(QString message) { onMessage(message); }
    void showMessage(QString message);

    void saveConfig();
    void loadConfig();
    void loadCTagsConfig(QString filename);
    void saveLog();

    void DrawStatus();

    void send();

    void on_actionClean_triggered();
    void on_actionAbout_triggered();

    void on_actionEnable_color_tags_activated();
    void on_actionRemove_log_messages_activated();
    void on_actionShow_message_time_activated();
    void on_actionRemove_server_log_triggered();

    void closeEvent(QCloseEvent *event);

    void on_playerlist_itemDoubleClicked(QListWidgetItem *item);
    void on_playerlist_itemPressed(QListWidgetItem *item);

    void on_actionKick_player_triggered();
    void on_actionForce_spectate_triggered();
    void on_actionKick_with_reason_triggered();
    void on_actionBan_player_triggered();
    void on_actionMute_player_triggered();
    void on_actionUnmute_player_triggered();

    void on_actionSave_log_triggered();
    void on_actionSave_log_html_triggered();

    void on_actionPlay_chat_sound_triggered();
    void on_actionImport_chat_sound_triggered();

    void on_actionReconnect_triggered();

    void on_actionMove_cursor_to_end_on_message_triggered();

    void on_actionAuto_save_logs_triggered();
    void on_actionSave_in_HTML_Document_triggered();
    void on_actionChoose_autosave_path_triggered();

    void on_actionSet_Font_triggered();

    void on_lineNickname_textChanged(const QString &arg1);

    void on_actionQuit_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionWatch_CVars_triggered();

    protected:
    virtual void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    //virtual void mousePressEvent(QMouseEvent *event);

private:

    Ui::RconWindow *ui;

    QUdpSocket socket;
};

#endif // RCONWINDOW_H
