#include "../include/MainWindow.h"

MainWindow::MainWindow() : stopwatch(this), wifiDialogue(this), shortcutLabel("WiFi: CTRL + W", this) {
    initDefaultPalette();
    resize(480, 320);
    setPalette(defaultPalette);
    setAutoFillBackground(true);

    shortcutLabel.setGeometry(0, 0, 125, 25);

    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QShortcut *wifiShortcut = new QShortcut(QKeySequence("Ctrl+W"), this); // Creating a shortcut
    connect(wifiShortcut, &QShortcut::activated, &wifiDialogue, &WiFiDialogue::show); // Setting the callback for the shortcut
}

void MainWindow::start() {
    show();
    stopwatch.show();
}
