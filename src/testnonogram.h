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
    void initHeartNonogram(Nonogram& n);
    void initCarNonogram(Nonogram& n);
    void fillRandomData(Nonogram* n);
private slots:
    void builtinNonograms();
    void saveLoad();
    void solve();
#endif // QT_DEBUG
};

#endif // TESTNONOGRAM_H
