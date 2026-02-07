#pragma once
#include <QListWidget>

class APList : public QListWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    APList(QWidget* parent);
public slots:
    void refresh();
};