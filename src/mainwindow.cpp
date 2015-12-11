#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newfiledialog.h"
#include "nonogramtabledelegate.h"
#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>
#include <QKeyEvent>
#include <QFileOpenEvent>
#include <QtConcurrent/QtConcurrent>

using namespace std;

Nonogram* createRandomNonogram(){
    const int width=15, height=20;
    Nonogram* n=new Nonogram(width, height);
    for(int r=0; r<height; r++)
        for(int c=0; c<width; c++)
            n->setData(r, c, (Nonogram::CellStatus)(rand()%3));
    for(int i=0; i<width; i++){
        int itemsCount=rand()%4+1;
        InfoListType items;
        for(int j=0; j<itemsCount; j++)
            items.append(rand()%9+1);
        n->setColumnInfo(i, items);
    }
    for(int i=0; i<height; i++){
        int itemsCount=rand()%6+1;
        InfoListType items;
        for(int j=0; j<itemsCount; j++)
            items.append(rand()%9+1);
        n->setRowInfo(i, items);
    }
    return n;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _nonogramModel(parent)
{
    ui->setupUi(this);
    ui->tableView->setModel(&_nonogramModel);
    NonogramTableDelegate* delegate=new NonogramTableDelegate;
    delegate->setCellSize(QSize(24, 24));
    delegate->setBorderWidth(2);
    delegate->setBorderColor(Qt::black);
    delegate->setGroupSize(5);
    ui->tableView->setItemDelegate(delegate);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setStyleSheet("QTableView { gridline-color: black } ");

//    setNonogram(new Nonogram(15, 20));
    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteCell()));
}

MainWindow::~MainWindow(){

}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    QFileOpenEvent* fileEvent=dynamic_cast<QFileOpenEvent*>(event);
    if (fileEvent){
        open(fileEvent->file());
        return true;
    }
    else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::deleteCell()
{
    QModelIndex idx = ui->tableView->currentIndex();
    if (idx.isValid())
       ui->tableView->model()->setData(idx, "");
}

void MainWindow::newFile(){
    Ui::Dialog newFileDialog;
    QDialog dialog(this, Qt::WindowCloseButtonHint| Qt::CustomizeWindowHint| Qt::WindowTitleHint);
    newFileDialog.setupUi(&dialog);
    if (dialog.exec()==QDialog::Accepted)
        setNonogram(new Nonogram(newFileDialog.widthSpinBox->value(), newFileDialog.heightSpinBox->value()));
}

void MainWindow::open(const QString& fileName){
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)){
        QDataStream out(&file);
        Nonogram* nonogram=new Nonogram;
        out >> *nonogram;
        if (nonogram->isValid())
            setNonogram(nonogram);
        else {
            QMessageBox mbox(QMessageBox::Critical, tr("Open File"), tr("Open File Failed"), QMessageBox::Ok, this);
            mbox.exec();
        }
    }
}

void MainWindow::save(const QString& fileName){
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)){
        QDataStream out(&file);
        out << *_nonogram;
    }
}

void MainWindow::openFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Nonogram Files (*.nom)"));
    if (!fileName.isEmpty())
        open(fileName);
}

void MainWindow::saveFile(){
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"), "", tr("Nonogram Files (*.nom)"));
    if (!fileName.isEmpty())
        save(fileName);
}

void MainWindow::solve(){
    if (!_nonogram->isSolveable()){
        QMessageBox mbox(QMessageBox::Critical, tr("Solve"), tr("Nonogram contains wrong data, please check"), QMessageBox::Ok, this);
        mbox.exec();
    }
    else{
        QtConcurrent::run(_nonogram.get(), &Nonogram::solve);
    }
}

void MainWindow::setNonogram(Nonogram* nonogram){
    _nonogram=shared_ptr<Nonogram>(nonogram);
    _nonogramModel.setNonogram(nonogram);
    ui->action_save->setEnabled(nonogram!=NULL);
    ui->action_solve->setEnabled(nonogram!=NULL);
}
