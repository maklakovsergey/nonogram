#include "numberlineedit.h"
#include <QRegExpValidator>
#include <QKeyEvent>

NumberLineEdit::NumberLineEdit(QWidget* parent):QLineEdit(parent){
    setValidator(new QRegExpValidator(QRegExp("[0-9]{,2}"), parent));
}

NumberLineEdit::~NumberLineEdit(){
}

bool NumberLineEdit::event(QEvent *event){
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        int key=keyEvent->key();
        if (key==Qt::Key_Left || key==Qt::Key_Right)
            return false;
    }
    return QLineEdit::event(event);
}
