#include "../include/APList.h"

#include <qcoreapplication.h>

APList::APList(QWidget* parent) : QListWidget(parent) {

    // Find and set wlan0
    NetworkManager::Device::List interfaces = NetworkManager::networkInterfaces();
    for (const auto& interface : interfaces) {
        if (interface->interfaceName() == "wlan0") {
            wlan0 = interface.dynamicCast<NetworkManager::WirelessDevice>();
            break;
        }
    }
    if (!wlan0) {
        qFatal("Could not find network interface");
    }

    // Set callback for when a new AP appears
    connect(wlan0.data(), &NetworkManager::WirelessDevice::accessPointAppeared, this, &APList::apAppeared);
    refresh();
}

void APList::refresh() {
    clear();

    // Add all already-known APs
    QStringList accessPoints = wlan0->accessPoints();
    for (const QString& apID : accessPoints) {
        NetworkManager::AccessPoint::Ptr ap = wlan0->findAccessPoint(apID);
        if (ap && !ap->ssid().isEmpty()) {
            addItem(ap->ssid());
        }
    }

    wlan0->requestScan();
}

void APList::apAppeared(const QString& apID) {
    NetworkManager::AccessPoint::Ptr ap = wlan0->findAccessPoint(apID);

    if (!ap) { return; }

    if (item(0)->text() == scanningText) {
        clear();
    }

    addItem(ap->ssid());
}
