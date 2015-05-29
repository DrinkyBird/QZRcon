#ifndef CVARWATCHWINDOW_H
#define CVARWATCHWINDOW_H

#include <QWidget>
#include "rconwindow.h"

namespace Ui {
    class CvarWatchWindow;
}

class CvarWatchWindow : public QWidget
{
        Q_OBJECT

    public:
        explicit CvarWatchWindow(RconWindow *rconwindow = 0);
        ~CvarWatchWindow();

        QStringList watching;

    private slots:
        void on_watchCvarButton_clicked();

        void onCvarAlreadyWatched(QString name);

    private:
        Ui::CvarWatchWindow *ui;
        RconWindow *rconwindow;
};

#endif // CVARWATCHWINDOW_H
