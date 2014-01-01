#ifndef BANWINDOW_H
#define BANWINDOW_H

#include "rconwindow.h"
#include <QWidget>

namespace Ui {
class banwindow;
}

class banwindow : public QWidget
{
    Q_OBJECT

public:
    explicit banwindow(RconWindow *rconwindow);
    ~banwindow();

private slots:
    void on_cancel_clicked();
    void on_ban_clicked();

private:
    Ui::banwindow *ui;
    RconWindow *rconwindow;

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // BANWINDOW_H
