#include "../include/WiFiDialogue.h"

#include "APList.h"

WiFiDialogue::WiFiDialogue(QWidget* parent) : QDialog(parent), label("Connect to Wi-Fi", this),
                                              refreshButton("Refresh", this), apList(this), passwordField(this),
                                              cancelButton("Cancel", this), connectButton("Connect", this) {

    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addWidget(&connectButton);

    mainLayout.addWidget(&label);
    mainLayout.addWidget(&refreshButton);
    mainLayout.addWidget(&apList);
    mainLayout.addWidget(&passwordField);
    mainLayout.addLayout(&buttonLayout);

    // Setting callbacks
    connect(&refreshButton, &QPushButton::clicked, &apList, &APList::refresh);
    connect(&cancelButton, &QPushButton::clicked, this, &QDialog::hide);
    connect(&connectButton, &QPushButton::clicked, this, &WiFiDialogue::connectButtonPressed);

    setLayout(&mainLayout);
    setGeometry(25, 25, 430, 270);
}

void WiFiDialogue::keyPressEvent(QKeyEvent* event) {

    // Fixes ESC to exit dialogue
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        hide();
        return;
    }

    QDialog::keyPressEvent(event);
}

void WiFiDialogue::connectButtonPressed() {

    NetworkManager::Connection::List connections = NetworkManager::listConnections();
    // Removes all other connections for simplicity
    for (const NetworkManager::Connection::Ptr &connection : connections) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();

        if (!settings) { continue; }

        if (settings->connectionType() != NetworkManager::ConnectionSettings::Wireless) { continue; }

        connection->remove();
    }

    QString ssid = apList.selectedItems()[0]->text();
    QString password = passwordField.text();

    // Create a settings map
    NMVariantMapMap settingsMap;

    // Connection settings
    QVariantMap connectionMap;
    connectionMap.insert("id", ssid);
    connectionMap.insert("uuid", QUuid::createUuid().toString().remove('{').remove('}'));
    connectionMap.insert("type", "802-11-wireless");
    connectionMap.insert("autoconnect", true);
    connectionMap.insert("interface-name", "wlan0");
    settingsMap.insert("connection", connectionMap);

    // Wireless settings
    QVariantMap wirelessMap;
    wirelessMap.insert("ssid", ssid.toUtf8());
    wirelessMap.insert("mode", "infrastructure");
    settingsMap.insert("802-11-wireless", wirelessMap);

    // Wireless security settings
    QVariantMap securityMap;
    securityMap.insert("key-mgmt", "wpa-psk");
    securityMap.insert("psk", password);
    securityMap.insert("psk-flags", 0);
    settingsMap.insert("802-11-wireless-security", securityMap);

    // IPv4 settings
    QVariantMap ipv4Map;
    ipv4Map.insert("method", "auto");
    ipv4Map.insert("never-default", true);
    settingsMap.insert("ipv4", ipv4Map);

    // IPv6 settings
    QVariantMap ipv6Map;
    ipv6Map.insert("method", "auto");
    ipv6Map.insert("never-default", true);
    settingsMap.insert("ipv6", ipv6Map);

    // Add the connection using the raw map
    NetworkManager::addConnection(settingsMap);

    hide();
}
