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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _nonogram(nullptr),
    _nonogramModel(parent)
{
    ui->setupUi(this);
    ui->tableView->setModel(&_nonogramModel);
    NonogramTableDelegate* delegate=new NonogramTableDelegate(this);
    delegate->setCellSize(QSize(24, 24));
    delegate->setBorderWidth(2);
    delegate->setBorderColor(Qt::black);
    delegate->setGroupSize(5);
    ui->tableView->setItemDelegate(delegate);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setStyleSheet("QTableView { gridline-color: black } ");
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::refreshEditMenu);

    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    connect(shortcut, &QShortcut::activated, this, &MainWindow::deleteCell);

    connect(&_solvingTask, &QFutureWatcher<void>::finished, this, &MainWindow::solved);
    _closeAfterSolve=false;
    scale=0;
}

MainWindow::~MainWindow(){
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    //for opening files from OS X finder
    QFileOpenEvent* fileEvent=dynamic_cast<QFileOpenEvent*>(event);
    if (fileEvent){
        open(fileEvent->file());
        return true;
    }
    else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event){
    if (_solvingTask.isRunning()){
        qDebug()<<"abort";
        _closeAfterSolve=true;
        _solver.abort();
        event->ignore();
    }
    else
        event->accept();
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
        setNonogram(new Nonogram(newFileDialog.widthSpinBox->value(), newFileDialog.heightSpinBox->value(), this));
}

void MainWindow::open(const QString& fileName){
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)){
        QDataStream out(&file);
        Nonogram* nonogram=new Nonogram(this);
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
    else{
        enableEditing(false);
        _solvingTask.setFuture(QtConcurrent::run(&_solver, &NonogramSolver::solve, _nonogram));
    }
}

void MainWindow::solved(){
    if (_closeAfterSolve)
        close();
}

void MainWindow::refreshEditMenu(){
	bool canEditRow = false;
	bool canEditColumn = false;
    QModelIndex index=ui->tableView->selectionModel()->currentIndex();
	if (index.isValid()) {
		canEditRow = index.row() >= _nonogramModel.dataBlockRow();
		canEditColumn = index.column() >= _nonogramModel.dataBlockColumn();
	}
    ui->action_column_insert_left->   setEnabled(_enableEditing && canEditColumn);
    ui->action_column_insert_right->  setEnabled(_enableEditing && canEditColumn);
    ui->action_column_remove_current->setEnabled(_enableEditing && canEditColumn);
    ui->action_row_insert_above->     setEnabled(_enableEditing && canEditRow);
    ui->action_row_insert_below->     setEnabled(_enableEditing && canEditRow);
    ui->action_row_remove_current->   setEnabled(_enableEditing && canEditRow);
}

void MainWindow::setNonogram(Nonogram* nonogram){
    if (_nonogram)
        delete _nonogram;
    _nonogram=nonogram;
    _nonogramModel.setNonogram(nonogram);
    _nonogramModel.setNonogramSolver(&_solver);
    enableEditing(true);
    ui->action_save->setEnabled(nonogram!=nullptr);
    ui->action_solve->setEnabled(nonogram!=nullptr);
}

void MainWindow::enableEditing(bool enableEditing){
    if (enableEditing!=_enableEditing){
        _enableEditing=enableEditing;
        _nonogramModel.setEditing(_enableEditing);
        _nonogramModel.refreshInfo();
        refreshEditMenu();
    }
}
