#include "mainwindow.h"
#include "huffman.h"

int main(int argc, char *argv[])
{
    HUFFMAN_Construct(); // atexit(HUFFMAN_Destruct) is called inside.

    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();

    return a.exec();
}
