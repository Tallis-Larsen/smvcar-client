#pragma once
#include <QWidget>
#include <QShortcut>
#include <QLabel>
#include "WiFiDialogue.h"
#include "DefaultPalette.h"
#include "LapStopwatch.h"

class MainWindow : public QWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    MainWindow();
    void start();
private:
    QLabel shortcutLabel;
    LapStopwatch stopwatch;
    WiFiDialogue wifiDialogue;
};