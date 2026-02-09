#include "../include/MainWindow.h"

MainWindow::MainWindow() : stopwatch(this), wifiDialogue(this),
    shortcutLabel("WiFi: CTRL + W", this), stateIndicator(this) {

    initDefaultPalette();
    resize(480, 320);
    setPalette(defaultPalette);
    setAutoFillBackground(true);

    shortcutLabel.setGeometry(0, 0, 125, 25);
    stateIndicator.setGeometry(200, 0, 280, 25);
    stateIndicator.setAlignment(Qt::AlignRight);

    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QShortcut *wifiShortcut = new QShortcut(QKeySequence("Ctrl+W"), this); // Creating a shortcut
    connect(wifiShortcut, &QShortcut::activated, &wifiDialogue, &WiFiDialogue::show); // Setting the callback for the shortcut

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

    // Create callback to update state indicator
    connect(wlan0.data(), &NetworkManager::Device::stateChanged, this, &MainWindow::wifiStateChanged);
    // Initially call function to ensure the state doesn't say "Disconnected" when it actually connected before the program started
    wifiStateChanged(wlan0->state(), wlan0->state()); // oldState argument doesn't matter, it's just required for the signal to be accepted

}

void MainWindow::start() {
    show();
    stopwatch.show();
}

void MainWindow::wifiStateChanged(NetworkManager::Device::State newState, NetworkManager::Device::State oldState) {
    Q_UNUSED(oldState)

    // Copy palette so it can be modified
    QPalette palette = stateIndicator.palette();

    // Cool C++ feature I learned: case fallthrough. Turns out you can do this and it works.
    switch (newState) {
        case NetworkManager::Device::UnknownState:
        case NetworkManager::Device::Unmanaged:
        case NetworkManager::Device::Unavailable:
        case NetworkManager::Device::Disconnected:
        case NetworkManager::Device::Failed:
        case NetworkManager::Device::Deactivating:
            stateIndicator.setText("Disconnected");
            palette.setColor(QPalette::WindowText, QColor(255, 0, 0)); // Red
            break;
        case NetworkManager::Device::Preparing:
        case NetworkManager::Device::ConfiguringIp:
        case NetworkManager::Device::ConfiguringHardware:
        case NetworkManager::Device::NeedAuth:
        case NetworkManager::Device::CheckingIp:
        case NetworkManager::Device::WaitingForSecondaries:
            stateIndicator.setText("Connecting");
            palette.setColor(QPalette::WindowText, QColor(255, 255, 0)); // Yellow
            break;
        case NetworkManager::Device::Activated:
            stateIndicator.setText("Connected");
            palette.setColor(QPalette::WindowText, QColor(0, 255, 0)); // Green
            break;
    }

    // Re-apply the palette
    stateIndicator.setPalette(palette);

}
