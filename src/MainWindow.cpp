#include "../include/MainWindow.h"

MainWindow::MainWindow() : stopwatch(this) {
    initDefaultPalette();
    resize(480, 320);
    setPalette(defaultPalette);
    setAutoFillBackground(true);

    stopwatch.setPalette(defaultPalette);

    show();
    stopwatch.show();
}
