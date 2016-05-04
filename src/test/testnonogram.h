#ifndef TESTNONOGRAM_H
#define TESTNONOGRAM_H

#include <QObject>

class TestNonogram : public QObject
{
    Q_OBJECT
public:
    explicit TestNonogram(QObject *parent = 0);

signals:

private slots:
    void saveLoad();
};

#endif // TESTNONOGRAM_H
