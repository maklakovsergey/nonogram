#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newfiledialog.h"
#include "nonogramtabledelegate.h"
#include "exportimagedialog.h"
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

    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    connect(shortcut, &QShortcut::activated, this, MainWindow::deleteCell);

    scale=0;
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

void MainWindow::fileNew(){
    Ui::NewFileDialog newFileDialog;
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

void MainWindow::fileOpen(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Nonogram Files (*.nom)"));
    if (!fileName.isEmpty())
        open(fileName);
}

void MainWindow::fileSave(){
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"), "", tr("Nonogram Files (*.nom)"));
    if (!fileName.isEmpty())
        save(fileName);
}

void MainWindow::fileExportAsImage(){
    ExportImageDialog dialog(*_nonogram, this);
    if (dialog.exec()==QDialog::Accepted){
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Image"), "", tr("Image Files (*.png)"));
        if (!fileName.isEmpty())
            dialog.image().save(fileName, "png");
    }
}

void MainWindow::editRowInsertAbove(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int rowNo=selectedIndex.row()-_nonogramModel.dataBlockRow();
    if (rowNo<0)
        return;
    _nonogram->insertRow(rowNo);
    ui->tableView->setCurrentIndex(_nonogramModel.index(selectedIndex.row()+1, selectedIndex.column()));
}

void MainWindow::editRowInsertBelow(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int rowNo=selectedIndex.row()-_nonogramModel.dataBlockRow();
    if (rowNo<0)
        return;
    _nonogram->insertRow(rowNo+1);
}

void MainWindow::editRowRemoveCurrent(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int rowNo=selectedIndex.row()-_nonogramModel.dataBlockRow();
    if (rowNo<0)
        return;
    _nonogram->removeRow(rowNo);
}

void MainWindow::editColumnInsertLeft(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int columnNo=selectedIndex.column()-_nonogramModel.dataBlockColumn();
    if (columnNo<0)
        return;
    _nonogram->insertColumn(columnNo);
    ui->tableView->setCurrentIndex(_nonogramModel.index(selectedIndex.row(), selectedIndex.column()+1));
}

void MainWindow::editColumnInsertRight(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int columnNo=selectedIndex.column()-_nonogramModel.dataBlockColumn();
    if (columnNo<0)
        return;
    _nonogram->insertColumn(columnNo+1);
}

void MainWindow::editColumnRemoveCurrent(){
    QModelIndex selectedIndex=ui->tableView->currentIndex();
    if (!selectedIndex.isValid())
        return;
    int columnNo=selectedIndex.column()-_nonogramModel.dataBlockColumn();
    if (columnNo<0)
        return;
    _nonogram->removeColumn(columnNo);
}

void MainWindow::solve(){
    if (!_nonogram->isSolveable()){
        QMessageBox mbox(QMessageBox::Critical, tr("Solve"), tr("Nonogram contains wrong data, please check"), QMessageBox::Ok, this);
        mbox.exec();
    }
    else
        QtConcurrent::run(_nonogram.get(), &Nonogram::solve);
}

void MainWindow::setNonogram(Nonogram* nonogram){
    _nonogram=shared_ptr<Nonogram>(nonogram);
    _nonogramModel.setNonogram(nonogram);
    ui->action_save->setEnabled(nonogram!=NULL);
    ui->action_solve->setEnabled(nonogram!=NULL);
    adjustTableSize();
}

void MainWindow::adjustTableSize(){
    /*QRect rect = ui->tableView->geometry();
    rect.setWidth(ui->tableView->horizontalHeader()->length()+2*ui->tableView->frameWidth());
    rect.setHeight(ui->tableView->verticalHeader()->length()+2*ui->tableView->frameWidth());
    ui->tableView->setGeometry(rect);*/
}
