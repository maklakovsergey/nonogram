#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_newfiledialog.h"
#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <assert.h>
#include "nonogramtabledelegate.h"
#include <QShortcut>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _nonogramModel(new NonogramModel)
{
    ui->setupUi(this);
    ui->tableView->setModel(_nonogramModel.get());
    ui->tableView->setItemDelegate(new NonogramTableDelegate());
    const int width=15, height=20;
    Nonogram* n=new Nonogram(width, height);
    for(int i=0; i<width*height; i++)
            n->data()[i]=(Nonogram::CellStatus)(rand()%3);
    for(int i=0; i<width; i++){
        int itemsCount=rand()%4+1;
        for(int j=0; j<itemsCount; j++)
            n->columnInfo()[i].append(rand()%10);
    }
    for(int i=0; i<height; i++){
        int itemsCount=rand()%6+1;
        for(int j=0; j<itemsCount; j++)
            n->rowInfo()[i].append(rand()%10);
    }
    setNonogram(n);
    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteCell()));



    //some unit testing save/load
    /*const int width=15, height=20;
    Nonogram n(width, height);
    for(int i=0; i<width*height; i++)
            n.data()[i]=(Nonogram::CellStatus)(rand()%3);
    for(int i=0; i<width; i++){
        int itemsCount=rand()%5+1;
        for(int j=0; j<itemsCount; j++)
            n.columnInfo()[i].append(rand()%10);
    }
    for(int i=0; i<height; i++){
        int itemsCount=rand()%5+1;
        for(int j=0; j<itemsCount; j++)
            n.rowInfo()[i].append(rand()%10);
    }
    QByteArray storage(10240, 0);
    QDataStream writestream(&storage, QIODevice::WriteOnly);
    n.save(writestream);
    QDataStream readstream(&storage, QIODevice::ReadOnly);
    Nonogram m(readstream);
    assert(m.width()==n.width());
    assert(m.height()==n.height());
    for(int i=0; i<width*height; i++)
        assert(m.data()[i]==n.data()[i]);
    for(int i=0; i<m.width(); i++){
        int itemsCount=m.columnInfo()[i].count();
        assert(n.columnInfo()[i].count()==itemsCount);
        for(int j=0; j<itemsCount; j++)
            assert(n.columnInfo()[i][j]==m.columnInfo()[i][j]);
    }
    for(int i=0; i<m.height(); i++){
        int itemsCount=m.rowInfo()[i].count();
        assert(n.rowInfo()[i].count()==itemsCount);
        for(int j=0; j<itemsCount; j++)
            assert(n.rowInfo()[i][j]==m.rowInfo()[i][j]);
    }*/
}

MainWindow::~MainWindow(){

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
    else
        qDebug()<<"new cancelled";
}

void MainWindow::openFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Nomogram Files (*.nom)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)){
            QDataStream out(&file);
            try{
                setNonogram(new Nonogram(out));
            }
            catch(exception){
                QMessageBox mbox(QMessageBox::Critical, tr("Open File"), tr("Open File Failed"), QMessageBox::Ok, this);
                mbox.exec();
            }
        }
    }
}

void MainWindow::saveFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Nomogram Files (*.nom)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            _nonogram->save(out);
        }
    }
}

void MainWindow::setNonogram(Nonogram* nonogram){
    _nonogram=shared_ptr<Nonogram>(nonogram);
    _nonogramModel->setNonogram(nonogram);
    ui->action_save->setEnabled(nonogram!=NULL);
}
