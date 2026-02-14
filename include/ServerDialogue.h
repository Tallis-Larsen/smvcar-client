#pragma once
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include "ServerAPI.h"

class ServerDialogue : public QDialog {
    Q_OBJECT
public:
    ServerDialogue(QWidget* parent);
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    QLabel label;
    QLineEdit urlField;
    QPushButton cancelButton;
    QPushButton connectButton;
    QVBoxLayout mainLayout = QVBoxLayout();
    QHBoxLayout buttonLayout = QHBoxLayout();

    void connectButtonPressed();
};
