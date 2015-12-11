#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <memory>
#include "nonogram.h"
#include "nonogrammodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void open(const QString& file);
    void save(const QString& file);
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void newFile();
    void openFile();
    void saveFile();
    void deleteCell();
    void solve();
private:
    std::shared_ptr<Ui::MainWindow> ui;
    std::shared_ptr<Nonogram> _nonogram;
    NonogramModel _nonogramModel;

    void setNonogram(Nonogram* nonogram);
};

#endif // MAINWINDOW_H
