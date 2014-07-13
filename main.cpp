#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //std::thread t1(FileChecker);


    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    (*w).setWindowTitle("GoAT - GUI");
    (*w).setWindowIcon(QIcon(":/icons/data/Fixicon-Farm-Goat.ico"));


    (*w).showMaximized();

   // future = QtConcurrent::run(FileChecker);
    return a.exec();
}

