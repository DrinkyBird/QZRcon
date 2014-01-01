#ifndef KICKWINDOW_H
#define KICKWINDOW_H

#include "rconwindow.h"
#include <QWidget>

namespace Ui {
class kickwindow;
}

class kickwindow : public QWidget
{
    Q_OBJECT

public:
    explicit kickwindow(RconWindow *rconwindow);
    ~kickwindow();
    
private slots:
    void on_kick_clicked();
    void on_cancel_clicked();

private:
    Ui::kickwindow *ui;
    RconWindow *rconwindow;

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // KICKWINDOW_H
