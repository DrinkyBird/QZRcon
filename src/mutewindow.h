#ifndef MUTEWINDOW_H
#define MUTEWINDOW_H

#include <QWidget>
#include "rconwindow.h"

namespace Ui {
class mutewindow;
}

class mutewindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit mutewindow(RconWindow *rconwindow = 0);
    ~mutewindow();
    
private slots:
    void on_cancel_clicked();
    void on_mute_clicked();

private:
    Ui::mutewindow *ui;
    RconWindow *rconwindow;

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // MUTEWINDOW_H
