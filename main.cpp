#include "connection.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("gloggcat");
    if (!createConnection())
        return EXIT_FAILURE;
    MainWindow w;
    w.show();
    return a.exec();
}
