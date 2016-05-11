#include "exportimagedialog.h"
#include "ui_exportimagedialog.h"

ExportImageDialog::ExportImageDialog(const Nonogram& nonogram, QWidget *parent) :
    QDialog(parent,  Qt::WindowCloseButtonHint| Qt::CustomizeWindowHint| Qt::WindowTitleHint),
    ui(new Ui::ExportImageDialog),
    _nonogram(nonogram),
    _model(this),
    _widget(nullptr)
{
    ui->setupUi(this);

    _model.setEditing(false);
    _model.setNonogram(&_nonogram);
    _delegate=new NonogramTableDelegate(this);
    _delegate->setCellSize(QSize(24, 24));
    _delegate->setBorderWidth(2);
    _delegate->setBorderColor(Qt::black);
    _delegate->setGroupSize(5);
    _tableView.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _tableView.horizontalHeader()->setMinimumSectionSize(0);
    _tableView.horizontalHeader()->setVisible(false);
    _tableView.verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _tableView.verticalHeader()->setMinimumSectionSize(0);
    _tableView.verticalHeader()->setVisible(false);
    _tableView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _tableView.setItemDelegate(_delegate);
    _tableView.setModel(&_model);
    _tableView.setStyleSheet("QTableView { background: red; gridline-color: black } ");
    _tableView.setFrameShape(QFrame::NoFrame);
    ui->graphicsView->setScene(new QGraphicsScene(this));

    refreshImage();
}

ExportImageDialog::~ExportImageDialog(){
    delete ui;
    delete _delegate;
}

void ExportImageDialog::updateFit(){
    ui->graphicsView->fitInView(_widget, Qt::KeepAspectRatio);
}

void ExportImageDialog::resizeEvent(QResizeEvent *event){
    QDialog::resizeEvent(event);
    updateFit();
}

void ExportImageDialog::showEvent(QShowEvent *event){
    QDialog::showEvent(event);
    updateFit();
}

void ExportImageDialog::refreshImage(){
    bool reloadData=false;

    bool showGrid=ui->gridLinesCheckBox->checkState()== Qt::Checked;
    if (showGrid != _delegate->borderVisible()){
        _delegate->setBorderVisible(showGrid);
        _tableView.setShowGrid(showGrid);
        reloadData=true;
    }

    bool showHeader=ui->headersCheckBox->checkState()==Qt::Checked;
    if (showHeader != _model.headerVisible()){
        _model.setHeaderVisible(showHeader);
        reloadData=true;
    }
    if (reloadData || !_widget){
        _model.refreshInfo();
        QRect rect = _tableView.geometry();
        rect.setWidth(_tableView.horizontalHeader()->length()+2*_tableView.frameWidth());
        rect.setHeight(_tableView.verticalHeader()->length()+2*_tableView.frameWidth());
        _tableView.setGeometry(rect);

        QPixmap image(rect.width(), rect.height());
        QPainter painter(&image);
        _tableView.render(&painter);

        if (_widget)
            ui->graphicsView->scene()->removeItem(_widget);
        _widget=ui->graphicsView->scene()->addPixmap(image);
        ui->graphicsView->setSceneRect(_widget->boundingRect());
        _widget->setTransformationMode(Qt::SmoothTransformation);
        updateFit();
    }
}
