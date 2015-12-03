#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newfiledialog.h"
#include <QDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->setModel(NULL);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile(){
    Ui::Dialog newFileDialog;
    QDialog dialog(this, Qt::WindowCloseButtonHint| Qt::CustomizeWindowHint| Qt::WindowTitleHint);
    newFileDialog.setupUi(&dialog);
    if (dialog.exec()==QDialog::Accepted){
        qDebug()<<"new"<<newFileDialog.widthSpinBox->value()<<"x"<<newFileDialog.heightSpinBox->value();
    }
    else
        qDebug()<<"new cancelled";
}

void MainWindow::openFile(){
    qDebug()<<"open";
}

void MainWindow::saveFile(){
    qDebug()<<"save";
}
