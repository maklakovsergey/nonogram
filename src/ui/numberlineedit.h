#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>

class NumberLineEdit:public QLineEdit{
    Q_OBJECT
public:
    NumberLineEdit(QWidget* parent=NULL);
    ~NumberLineEdit();
    bool event(QEvent *event);
};

#endif // NUMBERLINEEDIT_H
