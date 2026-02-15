#include "../include/ServerAPI.h"

ServerAPI::ServerAPI(QObject* parent) : QObject(parent) {
    connect(&webSocket, &QWebSocket::connected, this, &ServerAPI::onConnected);
    connect(&webSocket, &QWebSocket::disconnected, this, &ServerAPI::onDisconnected);
    connect(&webSocket, &QWebSocket::textMessageReceived, this, &ServerAPI::processMessage);
    // Initially load the saved base url
    setUrl(getUrl());
}

void ServerAPI::onConnected() {
    connected = true;
    emit serverStatusChanged(true);
    flushQueue();
}

void ServerAPI::onDisconnected() {
    connected = false;
    emit serverStatusChanged(false);
    QTimer::singleShot(3000, this, [this]() { webSocket.open(baseUrl); });
}

void ServerAPI::processMessage(const QString& message) {
    for (const QString& queueMessage : pendingMessages) {
        QJsonObject queueMessageObject = QJsonDocument::fromJson(queueMessage.toUtf8()).object();
        QJsonObject activeMessageObject = QJsonDocument::fromJson(message.toUtf8()).object();
        if (queueMessageObject["command_id"].toString() == activeMessageObject["command_id"].toString()) {
            pendingMessages.removeOne(queueMessage);
            return;
        }
    }

    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());

    if (document.isNull() || !document.isObject()) {
        qWarning() << "Received invalid JSON";
        return;
    }

    QJsonObject command = document.object();
    QString function = command["function"].toString();

    if (function == "setTargetLaps") {
        emit setTargetLaps(command["target_laps"].toInt());
    } else if (function == "setTargetTime") {
        std::chrono::minutes targetTime(command["target_time"].toInteger());
        emit setTargetTime(targetTime);
    } else if (function == "setId") {
        clientId = command["client_id"].toInt();
    } else if (function == "setPrefix") {
        messageIdPrefix = command["message_prefix"].toString();
    } else if (function == "startStopwatch") {
        QDateTime time = QDateTime::fromString(command["timestamp"].toString(), Qt::ISODateWithMs);
        emit addStopwatchStart(time, command["command_id"].toString());
    } else if (function == "stopStopwatch") {
        QDateTime time = QDateTime::fromString(command["timestamp"].toString(), Qt::ISODateWithMs);
        emit addStopwatchStop(time, command["command_id"].toString());
    } else if (function == "lap") {
        QDateTime time = QDateTime::fromString(command["timestamp"].toString(), Qt::ISODateWithMs);
        emit addLap(time, command["command_id"].toString());
    } else if (function == "reject") {
        emit removeLap(command["command_id"].toString());
    }

}

void ServerAPI::sendMessage(const QJsonObject& message) {
    QString messageString = QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    flushQueue();
    pendingMessages.enqueue(messageString);
    if (connected) {
        webSocket.sendTextMessage(messageString);
    }
}

QString ServerAPI::nextCommandId() {
    return messageIdPrefix + QString::number(nextMessageId++);
}

void ServerAPI::flushQueue() {
    if (!connected) { return; }

    for (const QString& message : pendingMessages) {
        webSocket.sendTextMessage(message);
    }
}

void ServerAPI::setUrl(const QString& url) {
    savedUrlSetting.setValue("base_url", url);
    baseUrl = url;
    webSocket.open(baseUrl);
}

QString ServerAPI::getUrl() {
    return savedUrlSetting.value("base_url", "").toString();
}

QString ServerAPI::lap(QDateTime time) {
    QJsonObject message;
    message["function"] = "lap";
    message["timestamp"] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message["command_id"] = commandId;
    sendMessage(message);
    return commandId;
}

QString ServerAPI::startStopwatch(QDateTime time) {
    QJsonObject message;
    message["function"] = "startStopwatch";
    message["timestamp"] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message["command_id"] = commandId;
    sendMessage(message);
    return commandId;
}

QString ServerAPI::stopStopwatch(QDateTime time) {
    QJsonObject message;
    message["function"] = "stopStopwatch";
    message["timestamp"] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message["command_id"] = commandId;
    sendMessage(message);
    return commandId;
}

void ServerAPI::resetStopwatch() {
    QJsonObject message;
    message["function"] = "resetStopwatch";
    message["command_id"] = nextCommandId();
    sendMessage(message);
}
