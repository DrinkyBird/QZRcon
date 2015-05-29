#include "mainwindow.h"
#include "huffman.h"

#ifdef HAVE_QT5
#include <QApplication>
#endif

int main(int argc, char *argv[])
{
    HUFFMAN_Construct(); // atexit(HUFFMAN_Destruct) is called inside.

    QApplication a(argc, argv);

#ifndef _WIN32 // only for non-windows
	QApplication::setWindowIcon(QIcon("QZRcon.png")); // is global for all application windows
#endif

    MainWindow *w = new MainWindow;
    w->show();


    return a.exec();
}
