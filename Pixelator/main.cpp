// Kaelin Hoang, u0984462 | Tyrus Draper, u1038756 | Megan Schnedar, u0924126 | Khanhly Nguyen, u0822847 | Hannah Bruns, u0629077

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
