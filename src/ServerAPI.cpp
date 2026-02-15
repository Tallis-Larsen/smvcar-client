#include "../include/ServerAPI.h"

const QString BASE_URL = "base_url";
const QString COMMAND_ID = "command_id";
const QString FUNCTION = "function";
const QString TIMESTAMP = "timestamp";
const QString REJECT = "reject";
const QString LAP = "lap";
const QString START_STOPWATCH = "startStopwatch";
const QString STOP_STOPWATCH = "stopStopwatch";
const QString RESET_STOPWATCH = "resetStopwatch";
const QString SET_PREFIX = "setPrefix";
const QString SET_TARGET_LAPS = "setTargetLaps";
const QString SET_TARGET_TIME = "setTargetTime";
const QString TARGET_LAPS = "target_laps";
const QString TARGET_TIME = "target_time";
const QString MESSAGE_PREFIX = "message_prefix";

ServerAPI::ServerAPI(QObject* parent) : QObject(parent) {
    connect(&webSocket, &QWebSocket::connected, this, &ServerAPI::onConnected);
    connect(&webSocket, &QWebSocket::disconnected, this, &ServerAPI::onDisconnected);
    connect(&webSocket, &QWebSocket::textMessageReceived, this, &ServerAPI::processMessage);

    connect(&keepAliveTimer, &QTimer::timeout, this, [this]() {
    if (webSocket.state() == QAbstractSocket::ConnectedState) { webSocket.ping(); }});

    // Initially load the saved base url
    setUrl(getUrl());
}

void ServerAPI::onConnected() {
    connected = true;
    emit serverStatusChanged(true);
    flushQueue();
    // 30-second ping interval to keep the connection alive
    keepAliveTimer.start(30000);
}

void ServerAPI::onDisconnected() {
    keepAliveTimer.stop();

    connected = false;
    emit serverStatusChanged(false);
    QTimer::singleShot(3000, this, [this]() { webSocket.open(baseUrl); });
}

void ServerAPI::processMessage(const QString& message) {

    for (int i = 0; i < pendingMessages.size(); i++) {
        QJsonObject queueMessageObject = QJsonDocument::fromJson(pendingMessages[i].toUtf8()).object();
        QJsonObject activeMessageObject = QJsonDocument::fromJson(message.toUtf8()).object();
        if (queueMessageObject[COMMAND_ID].toString() == activeMessageObject[COMMAND_ID].toString()) {
            if (activeMessageObject[FUNCTION].toString() == REJECT) {
                emit removeLap(activeMessageObject[COMMAND_ID].toString());
            }
            if (activeMessageObject[FUNCTION].toString() == RESET_STOPWATCH) {
                pendingMessages.clear();
                return;
            }
            pendingMessages.removeAt(i);
            return;
        }
    }

    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());

    if (document.isNull() || !document.isObject()) {
        qWarning() << "Received invalid JSON";
        return;
    }

    QJsonObject command = document.object();
    QString function = command[FUNCTION].toString();

    if (function == SET_TARGET_LAPS) {
        emit setTargetLaps(command[TARGET_LAPS].toInt());
    } else if (function == SET_TARGET_TIME) {
        std::chrono::minutes targetTime(command[TARGET_TIME].toInteger());
        emit setTargetTime(targetTime);
    } else if (function == SET_PREFIX) {
        messageIdPrefix = command[MESSAGE_PREFIX].toString();
    } else if (function == START_STOPWATCH) {
        QDateTime time = QDateTime::fromString(command[TIMESTAMP].toString(), Qt::ISODateWithMs);
        emit addStopwatchStart(time, command[COMMAND_ID].toString());
    } else if (function == STOP_STOPWATCH) {
        QDateTime time = QDateTime::fromString(command[TIMESTAMP].toString(), Qt::ISODateWithMs);
        emit addStopwatchStop();
    } else if (function == LAP) {
        QDateTime time = QDateTime::fromString(command[TIMESTAMP].toString(), Qt::ISODateWithMs);
        emit addLap(time, command[COMMAND_ID].toString());
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
        qDebug() << "Queue Flush:" << message;
    }
}

void ServerAPI::setUrl(const QString& url) {
    savedUrlSetting.setValue(BASE_URL, url);
    baseUrl = url;
    webSocket.open(baseUrl);
}

QString ServerAPI::getUrl() {
    return savedUrlSetting.value(BASE_URL, "").toString();
}

QString ServerAPI::lap(QDateTime time) {
    QJsonObject message;
    message[FUNCTION] = LAP;
    message[TIMESTAMP] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message[COMMAND_ID] = commandId;
    sendMessage(message);
    return commandId;
}

QString ServerAPI::startStopwatch(QDateTime time) {
    QJsonObject message;
    message[FUNCTION] = START_STOPWATCH;
    message[TIMESTAMP] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message[COMMAND_ID] = commandId;
    sendMessage(message);
    return commandId;
}

QString ServerAPI::stopStopwatch(QDateTime time) {
    QJsonObject message;
    message[FUNCTION] = STOP_STOPWATCH;
    message[TIMESTAMP] = time.toUTC().toString(Qt::ISODateWithMs);
    QString commandId = nextCommandId();
    message[COMMAND_ID] = commandId;
    sendMessage(message);
    return commandId;
}

void ServerAPI::resetStopwatch() {
    QJsonObject message;
    message[FUNCTION] = RESET_STOPWATCH;
    message[COMMAND_ID] = nextCommandId();
    sendMessage(message);
}
