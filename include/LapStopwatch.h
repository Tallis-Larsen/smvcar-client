#pragma once
#include <QTableWidget>
#include <QHeaderView>
#include <QElapsedTimer>
#include <QTimer>
#include <QLabel>

class LapStopwatch : public QWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    LapStopwatch(QWidget* parent);
public slots:
    void startStopReset();
    void lap();
private:
    QTableWidget table;
    QElapsedTimer timer;
    QTimer updateTimer;
    QLabel totalTime;
    std::chrono::nanoseconds lastLapTime;
    bool isRunning = false;

    void addRow();
    void updateTime();
};
