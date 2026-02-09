#pragma once
#include <QListWidget>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Device>

class APList : public QListWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    APList(QWidget* parent);
public slots:
    void refresh();
private:
    NetworkManager::WirelessDevice::Ptr wlan0;
    QString scanningText = "Scanning...";
private slots:
    void apAppeared(const QString& apID);
};