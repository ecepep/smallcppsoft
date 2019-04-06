#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QDebug>

#include "GUI/mainwindow.h"
#include "GUI/prop.h"
#include "dbconnection/dbconnection.h"

/**
 * @todo: different languages using tr undefined
 * @todo run doxygen and check comment's tag
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
