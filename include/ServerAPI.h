#pragma once
#include <QNetworkAccessManager>
#include <QSettings>

class ServerAPI : public QObject {
    Q_OBJECT
public:
    static ServerAPI& instance() {
        static ServerAPI instance;
        return instance;
    }
private:
    ServerAPI(QObject* parent = nullptr);
    QNetworkAccessManager networkAccessManager;
    QSettings savedUrlSetting = QSettings("SMV", "smvcar-client");
    QString baseUrl;
};