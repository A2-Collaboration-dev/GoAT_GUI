#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //std::thread t1(FileChecker);


    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;


    (*w).showMaximized();

   // future = QtConcurrent::run(FileChecker);
    return a.exec();
}

