#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "nonogram.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();

private:
    std::shared_ptr<Ui::MainWindow> ui;
    std::shared_ptr<Nonogram> _nonogram;

    void setNonogram(Nonogram* nonogram);
};

#endif // MAINWINDOW_H
