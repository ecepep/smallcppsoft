#include "qdebug.h"
#include <QSizePolicy>
#include <QTextBrowser>

#include "GUI/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PropWindow* prop = new PropWindow(ui->scrollArea, this);
    ui->scrollArea->setWidget(prop);
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == PropLoadFail::eventPLF) {
//        PropLoadFail *event = static_cast<PropLoadFail *>(event);
        QTextBrowser *replacement = new QTextBrowser(this);
        replacement->setText("You are done for now. ;)");
        ui->scrollArea->setWidget(replacement);
        return true;
    }

    return QWidget::event(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}
