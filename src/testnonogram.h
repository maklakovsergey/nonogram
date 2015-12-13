#ifndef TESTNONOGRAM_H
#define TESTNONOGRAM_H

#include <QObject>
#include "nonogram.h"

class TestNonogram : public QObject
{
    Q_OBJECT
public:
    explicit TestNonogram(QObject *parent = 0);

signals:

#ifdef QT_DEBUG
private:
    Nonogram heartNonogram();
    Nonogram carNonogram();
    Nonogram clefNonogram();
    void fillRandomData(Nonogram& n);
private slots:
    void saveLoad();
    void solve();
#endif // QT_DEBUG
};

#endif // TESTNONOGRAM_H
