#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "GUI/propwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool event(QEvent *event);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
