#pragma once
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>

class ServerAPI : public QObject {
    Q_OBJECT
public:
    static ServerAPI& instance() {
        static ServerAPI instance;
        return instance;
    }

    void setUrl(const QString& url);
    QString getUrl();

    bool connected = false;
private:
    ServerAPI(QObject* parent = nullptr);
    QNetworkAccessManager networkAccessManager;
    QNetworkAccessManager pingManager; // Just for the check-alive pings
    QSettings savedUrlSetting = QSettings("SMV", "smvcar-client");
    QString baseUrl;
    QTimer pingTimer;

    void setConnected(bool isConnected);
private slots:
    void pingServer();
    void onPingReply(QNetworkReply* reply);

signals:
    void serverStatusChanged(bool isConnected);
};