#ifndef EXPORTIMAGEDIALOG_H
#define EXPORTIMAGEDIALOG_H

#include <QDialog>
#include <QImage>
#include <QTableView>
#include "nonogram.h"
#include "nonogrammodel.h"
#include "nonogramtabledelegate.h"
#include <QGraphicsPixmapItem>

namespace Ui {
class ExportImageDialog;
}

class ExportImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportImageDialog(const Nonogram& nonogram, QWidget *parent = 0);
    ~ExportImageDialog();
    QImage image() const {return _widget->pixmap().toImage();}

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
protected slots:
    void refreshImage();
private:
    Ui::ExportImageDialog *ui;
    Nonogram _nonogram;
    QTableView _tableView;
    NonogramModel _model;
    QGraphicsPixmapItem* _widget;
    NonogramTableDelegate* _delegate;
    void updateFit();
};

#endif // EXPORTIMAGEDIALOG_H
