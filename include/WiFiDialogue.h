#pragma once
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QUuid>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include "APList.h"

class WiFiDialogue : public QDialog {
    Q_OBJECT
public:
    WiFiDialogue(QWidget* parent);
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    QLabel label;
    QPushButton refreshButton;
    APList apList;
    QLineEdit passwordField;
    QPushButton cancelButton;
    QPushButton connectButton;
    QVBoxLayout mainLayout = QVBoxLayout();
    QHBoxLayout buttonLayout = QHBoxLayout();
private slots:
    void connectButtonPressed();
};