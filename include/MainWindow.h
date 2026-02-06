#pragma once
#include <QWidget>
#include "WiFiDialogue.h"
#include "DefaultPalette.h"
#include "LapStopwatch.h"

class MainWindow : public QWidget {
public:
    MainWindow();
private:
    LapStopwatch stopwatch;
};