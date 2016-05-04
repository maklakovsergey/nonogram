#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QFutureWatcher>
#include "nonogram.h"
#include "nonogrammodel.h"
#include "nonogramsolver.h"

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
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileExportAsImage();
    void editRowInsertAbove();
    void editRowInsertBelow();
    void editRowRemoveCurrent();
    void editColumnInsertLeft();
    void editColumnInsertRight();
    void editColumnRemoveCurrent();
    void deleteCell();
    void refreshEditMenu();
    void solve();
    void solved();
protected:
    Ui::MainWindow* ui;
    Nonogram* _nonogram;
    NonogramSolver _solver;
    NonogramModel _nonogramModel;
    float scale;
    bool _enableEditing;
    bool _closeAfterSolve;
    QFutureWatcher<void> _solvingTask;

    void setNonogram(Nonogram* nonogram);

    void enableEditing(bool enableEditing);
    void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
