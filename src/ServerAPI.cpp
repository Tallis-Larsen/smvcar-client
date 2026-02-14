#include "../include/ServerAPI.h"

#include "MainWindow.h"

ServerAPI::ServerAPI(QObject* parent) : QObject(parent) {
    // Ping the server every 5 seconds to check if the connection is valid
    connect(&pingTimer, &QTimer::timeout, this, &ServerAPI::pingServer);
    connect(&pingManager, &QNetworkAccessManager::finished, this, &ServerAPI::onPingReply);
    pingTimer.start(5000);

    // Initially load the saved base url
    setUrl(getUrl());
}

void ServerAPI::setUrl(const QString& url) {
    savedUrlSetting.setValue("base_url", url);
    baseUrl = url;
    pingServer();
}

QString ServerAPI::getUrl() {
    return savedUrlSetting.value("base_url", "").toString();
}

void ServerAPI::pingServer() {
    QNetworkRequest request = QNetworkRequest(QUrl(baseUrl));
    request.setTransferTimeout(3000);
    pingManager.get(request);
}

void ServerAPI::onPingReply(QNetworkReply* reply) {
    // If there was no error, we assume that the ping was successful.
    bool isConnected = reply->error() == QNetworkReply::NoError;
    setConnected(isConnected);
    // Delete the reply to avoid memory leaks
    reply->deleteLater();
}

void ServerAPI::setConnected(bool isConnected) {
    if (connected != isConnected) {
        connected = isConnected;
        emit serverStatusChanged(isConnected);
    }
}