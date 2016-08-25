#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fun();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fun()
{
    int i = 1;
    int j = 0;
    i /= j;
}
