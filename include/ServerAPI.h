#pragma once
#include <QWebSocket>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QQueue>
#include <chrono>

class ServerAPI : public QObject {
    Q_OBJECT
public:
    static ServerAPI& instance() {
        static ServerAPI instance;
        return instance;
    }

    void setUrl(const QString& url);
    QString getUrl();
    QString lap(QDateTime time);
    QString startStopwatch(QDateTime time);
    QString stopStopwatch(QDateTime time);
    void resetStopwatch();

    bool connected = false;
private:
    ServerAPI(QObject* parent = nullptr);
    QWebSocket webSocket;
    QTimer keepAliveTimer;
    QQueue<QString> pendingMessages;
    QSettings savedUrlSetting = QSettings("SMV", "smvcar-client");
    QString baseUrl = "";
    int nextMessageId = 0;
    QString messageIdPrefix;
    int clientId;

    void onConnected();
    void onDisconnected();
    void processMessage(const QString& message);
    void sendMessage(const QJsonObject& message);
    QString nextCommandId();
    void flushQueue();
signals:
    void serverStatusChanged(bool isConnected);
    void setTargetLaps(int laps);
    void setTargetTime(std::chrono::minutes minutes);
    void addLap(QDateTime time, QString id);
    void removeLap(QString id);
    void addStopwatchStart(QDateTime time, QString id);
    void addStopwatchStop();
};