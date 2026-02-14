#include "../include/MainWindow.h"

MainWindow::MainWindow() : stopwatch(this), wifiDialogue(this), serverDialogue(this),
    shortcutLabel("WiFi Menu: CTRL + W\nServer Menu: CTRL + S", this), wifiStateIndicator(this),
    clickerStateIndicator(this), serverStateIndicator(this), clickerWatcher(this) {

    initDefaultPalette();
    resize(480, 320);
    setPalette(defaultPalette);
    setAutoFillBackground(true);

    shortcutLabel.setGeometry(280, 280, 200, 50);
    shortcutLabel.setAlignment(Qt::AlignRight);
    serverStateIndicator.setGeometry(280, 255, 200, 25);
    serverStateIndicator.setAlignment(Qt::AlignRight);
    clickerStateIndicator.setGeometry(280, 230, 200, 25);
    clickerStateIndicator.setAlignment(Qt::AlignRight);
    wifiStateIndicator.setGeometry(280, 205, 200, 25);
    wifiStateIndicator.setAlignment(Qt::AlignRight);

    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QShortcut* wifiShortcut = new QShortcut(QKeySequence("Ctrl+W"), this); // Creating a shortcut
    connect(wifiShortcut, &QShortcut::activated, &wifiDialogue, &WiFiDialogue::show); // Setting the callback for the shortcut

    // The same for the server dialogue
    QShortcut* serverShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(serverShortcut, &QShortcut::activated, &serverDialogue, &ServerDialogue::show);

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

    // Create a callback to update the state indicator
    connect(wlan0.data(), &NetworkManager::Device::stateChanged, this, &MainWindow::wifiStateChanged);
    // Initially call function to ensure the state doesn't say "Disconnected" when it actually connected before the program started
    wifiStateChanged(wlan0->state(), wlan0->state()); // oldState argument doesn't matter, it's just required for the signal to be accepted

    // Do the same for the server state indicator
    connect(&ServerAPI::instance(), &ServerAPI::serverStatusChanged, this, &MainWindow::setServerState);
    setServerState(ServerAPI::instance().connected);

    // Watching the kernel for new bluetooth devices
    clickerWatcher.addPath("/dev/input/");
    connect(&clickerWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::checkClickerConnected);

    // Call the function once to check if the clicker connected before the program started
    checkClickerConnected("/dev/input/");

    // Set connect signals from pressing button to controlling the stopwatch.
    connect(this, &MainWindow::topButtonPressed, &stopwatch, &LapStopwatch::lap);
    connect(this, &MainWindow::bottomButtonPressed, &stopwatch, &LapStopwatch::startStopReset);

}

void MainWindow::start() {
    show();
    stopwatch.show();
}

void MainWindow::wifiStateChanged(NetworkManager::Device::State newState, NetworkManager::Device::State oldState) {
    Q_UNUSED(oldState)

    QString color;
    QString statusText;

    // Cool C++ feature I learned: case fallthrough. Turns out you can do this and it works.
    switch (newState) {
        case NetworkManager::Device::UnknownState:
        case NetworkManager::Device::Unmanaged:
        case NetworkManager::Device::Unavailable:
        case NetworkManager::Device::Disconnected:
        case NetworkManager::Device::Failed:
        case NetworkManager::Device::Deactivating:
            statusText = "Disconnected";
            color = "#FF0000"; // Red
            break;
        case NetworkManager::Device::Preparing:
        case NetworkManager::Device::ConfiguringIp:
        case NetworkManager::Device::ConfiguringHardware:
        case NetworkManager::Device::NeedAuth:
        case NetworkManager::Device::CheckingIp:
        case NetworkManager::Device::WaitingForSecondaries:
            statusText = "Connecting";
            color = "#FFFF00"; // Yellow
            break;
        case NetworkManager::Device::Activated:
            statusText = "Connected";
            color = "#00FF00"; // Green
            break;
    }

    // Using fancy HTML to mix colors
    QString htmlStatus = QString("<font color ='white'>WiFi: </font><font color='%1'>%2</font>").arg(color, statusText);

    wifiStateIndicator.setText(htmlStatus);
}

void MainWindow::setServerState(bool isConnected) {
    QString color;
    QString statusText;

    if (isConnected) {
        statusText = "Connected";
        color = "#00FF00"; // Green
    } else {
        statusText = "Disconnected";
        color = "#FF0000"; // Red
    }

    // Using fancy HTML to mix colors
    QString htmlStatus = QString("<font color ='white'>Server: </font><font color='%1'>%2</font>").arg(color, statusText);

    serverStateIndicator.setText(htmlStatus);
}

// This function could look prettier, but for now it works.
void MainWindow::checkClickerConnected(const QString& path) {
    QDir deviceDirectory(path);
    QStringList filters("event*");
    deviceDirectory.setNameFilters(filters);

    // Iterate through the files in the directory and check for a device named "JX-03"
    for (const QString& file : deviceDirectory.entryList(QDir::System)) {
        QFile device(deviceDirectory.absoluteFilePath(file));

        // Open the device and skip if it fails
        if (!device.open(QIODevice::ReadOnly)) { continue; }

        // Creates a buffer to store the name in for reading. The '0' means it will initialize the buffer with '0's to start.
        QByteArray nameBuffer(256, 0);

        // Grab the name and check to make sure it's not empty
        if (ioctl(device.handle(), EVIOCGNAME(nameBuffer.size()), nameBuffer.data()) >= 0) {
            // Immediately converting the raw buffer to a QString
            QString deviceName = QString::fromLocal8Bit(nameBuffer.constData());

            if (deviceName.contains("JX-03", Qt::CaseInsensitive)) {

                // Close now so that we don't open twice during connectToClicker()
                device.close();

                if (!clickerConnected) {
                    // Just in case there's an existing clicker object
                    disconnectClicker();
                    // Don't need to worry about delete because it will die with the program
                    clicker = new QFile(deviceDirectory.absoluteFilePath(file));
                    connectToClicker();
                }

                return;
            }
        }
    }

    setClickerConnected(false);

}

void MainWindow::connectToClicker() {

    // Just in case the clicker is null
    if (!clicker) { return; }

    // Open the file and disconnect if it fails (new QFile() doesn't open automatically like stack allocation does)
    if (!clicker->open(QIODevice::ReadOnly)) {
        disconnectClicker();
        return;
    }

    // Get the current file flags
    int flags = fcntl(clicker->handle(), F_GETFL, 0);
    // Add a new flag that enables non-blocking mode so that we can call read() without waiting for data
    fcntl(clicker->handle(), F_SETFL, flags | O_NONBLOCK);

    // Grab (intercept) the bluetooth signals and disconnect if it fails
    if (ioctl(clicker->handle(), EVIOCGRAB, 1) < 0) {
        disconnectClicker();
        return;
    }

    // Delete the notifier if it already exists
    if (clickerNotifier) {
        clickerNotifier->setEnabled(false);
        delete clickerNotifier;
        clickerNotifier = nullptr;
    }

    // Create new notifier
    clickerNotifier = new QSocketNotifier(clicker->handle(), QSocketNotifier::Read, this);
    // Create a callback
    connect(clickerNotifier, &QSocketNotifier::activated, this, &MainWindow::recieveClickerInput);

    setClickerConnected(true);

}

void MainWindow::recieveClickerInput() {

    // Disable notifier to make sure it can't call this function again while it's running
    clickerNotifier->setEnabled(false);

    struct input_event inputEvent;

    int bytesRead;

    // Loop through all events in the buffer
    while (( bytesRead = read(clicker->handle(), &inputEvent, sizeof(inputEvent))) > 0) {

        // One of the buttons was pressed, now we wait for the next event for the direction
        if (inputEvent.type == EV_KEY && inputEvent.code == 330 && inputEvent.value == 1) {
            waitingForSwipe = true;
        }

        if (waitingForSwipe && inputEvent.type == EV_ABS && inputEvent.code == 1) {

            // If the value is below 1600, the top button was pressed. Otherwise, the bottom button was pressed.
            if (inputEvent.value < 1600) {
                emit topButtonPressed();
            } else {
                emit bottomButtonPressed();
            }

            waitingForSwipe = false;

        }

    }

    // Re-enable notifier
    clickerNotifier->setEnabled(true);

    // We assume a critical error while reading the file means the device disconnected, so this filters out non-critical errors.
    if (bytesRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        disconnectClicker();
    }

}

void MainWindow::setClickerConnected(bool isConnected) {

    clickerConnected = isConnected;

    QString color;
    QString statusText;

    if (isConnected) {
        statusText = "Connected";
        color = "#00FF00"; // Green
    } else {
        statusText = "Disconnected";
        color = "#FF0000"; // Red
    }

    // Using fancy HTML to mix colors
    QString htmlStatus = QString("<font color ='white'>Clicker: </font><font color='%1'>%2</font>").arg(color, statusText);

    clickerStateIndicator.setText(htmlStatus);

}

void MainWindow::disconnectClicker() {
    if (clickerNotifier) {
        clickerNotifier->setEnabled(false);
        delete clickerNotifier;
        clickerNotifier = nullptr;
    }
    if (clicker) {
        if (clicker->isOpen()) {
            // Release the grab
            ioctl(clicker->handle(), EVIOCGRAB, 0);
            clicker->close();
        }
        delete clicker;
        clicker = nullptr;
    }
    setClickerConnected(false);
}
