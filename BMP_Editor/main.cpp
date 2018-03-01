#include "mainwindow.h"
#include <QApplication>
#include <cstdlib>
int main(int argc, char *argv[])
{

    //llib::Header *header;
    //llib::Bitmap bitmap;

   // llib::BMPLib::readBitmap("test1.bmp",&header, ((llib::Bitmap*)(&bitmap)));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
