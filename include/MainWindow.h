#pragma once
#include <QWidget>
#include <QShortcut>
#include <QLabel>
#include <QFileSystemWatcher>
#include <QDir>
#include <QFileInfo>
#include <QSocketNotifier>
#include <fcntl.h>
#include <unistd.h>
#include <NetworkManagerQt/Device>
#include <linux/input.h>
#include "WiFiDialogue.h"
#include "ServerDialogue.h"
#include "DefaultPalette.h"
#include "LapStopwatch.h"
#include "ServerAPI.h"

class MainWindow : public QWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    MainWindow();
    void start();
private:
    QLabel shortcutLabel;
    QLabel wifiStateIndicator;
    QLabel clickerStateIndicator;
    QLabel serverStateIndicator;
    LapStopwatch stopwatch;
    WiFiDialogue wifiDialogue;
    ServerDialogue serverDialogue;
    NetworkManager::WirelessDevice::Ptr wlan0;
    QFileSystemWatcher clickerWatcher;
    QFile* clicker = nullptr;
    bool clickerConnected = false;
    QSocketNotifier* clickerNotifier = nullptr;
    bool waitingForSwipe = false;

    void connectToClicker();
    void setClickerConnected(bool isConnected);
    void disconnectClicker();
private slots:
    void wifiStateChanged(NetworkManager::Device::State newState, NetworkManager::Device::State oldState);
    void setServerState(bool isConnected);
    void checkClickerConnected(const QString& path);
    void recieveClickerInput();

// Signals aren't public or private and don't need an implementation, just declaration.
signals:
    void topButtonPressed();
    void bottomButtonPressed();
};