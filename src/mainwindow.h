#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common.h"
#include "rcon.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void lockGUI(bool lock);
    void setStatus(QString status);
    void showError(QString message);

    void on_buttonConnect_clicked();
    void beginConnecting(QHostInfo hostinfo);

    void onError(int e);
    void onStateChange(int state);

    void on_actionAbout_triggered();

    void on_actionLoad_config_triggered();
    void on_actionSave_config_triggered();

private:
    void loadConfig();
    void loadIDEConfig();
    void saveConfig();

protected:
  virtual void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    Rcon *rcon;
    QTimer *timeoutTimer;
};

#endif // MAINWINDOW_H
