#ifndef TESTSOLVE_H
#define TESTSOLVE_H

#include <QObject>

class TestSolve : public QObject
{
    Q_OBJECT
public:
    explicit TestSolve(QObject *parent = 0);

signals:

private slots:
    void solve();
};

#endif // TESTSOLVE_H
