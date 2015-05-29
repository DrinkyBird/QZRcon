#ifndef CVARWATCHWINDOW_H
#define CVARWATCHWINDOW_H

#include <QDialog>

namespace Ui {
    class CvarWatchWindow;
}

class CvarWatchWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit CvarWatchWindow(QWidget *parent = 0);
        ~CvarWatchWindow();

    private:
        Ui::CvarWatchWindow *ui;

    private slots:
        void on_watchButton_clicked();

        void onCvarWatched(QString cvar, QString value);
};

#endif // CVARWATCHWINDOW_H
