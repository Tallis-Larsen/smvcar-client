#pragma once
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QProgressBar>
#include "ServerAPI.h"
#include "LapProgressBar.h"

// Creating a centiseconds type set to 1/100th of a second
using centiseconds = std::chrono::duration<int, std::ratio<1, 100>>;

class LapStopwatch : public QWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    LapStopwatch(QWidget* parent);
public slots:
    void startStopReset();
    void start(QDateTime time, QString id);
    void stop();
    void lap();
    void addRow(const QDateTime& lapTime, const QString& commandId);
    void removeRow(const QString& commandId);
    void setTargetTime(std::chrono::minutes minutes);
    void setTargetLaps(int laps);
private:
    QTableWidget table;
    QTimer updateTimer;
    QProgressBar timeProgress;
    LapProgressBar lapProgress;
    QLabel totalTime;
    QLabel targetTimeDisplay;
    QLabel currentLapDisplay;
    QLabel targetLapsDisplay;
    bool isRunning = false;
    std::chrono::seconds targetLapTime = std::chrono::seconds{0};

    void recalculateTable();
    void updateTime();
    void updateColors(QTableWidgetItem* item, std::chrono::seconds lapDuration);
    void calculateTargetLapTime();
    void reset();
};
