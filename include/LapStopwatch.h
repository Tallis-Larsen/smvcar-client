#pragma once
#include <QTableWidget>
#include <QHeaderView>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include "ServerAPI.h"

// Creating a centiseconds type set to 1/100th of a second
using centiseconds = std::chrono::duration<int, std::ratio<1, 100>>;

class LapStopwatch : public QWidget {
    Q_OBJECT // This is custom to Qt and requires a custom compiler step. It designates the class as a Qt object.
public:
    LapStopwatch(QWidget* parent);
public slots:
    void startStopReset();
    void start();
    void stop();
    void lap();
    void addRow(const QDateTime& lapTime, const QString& commandId);
    void removeRow(const QString& commandId);
private:
    QTableWidget table;
    QTimer updateTimer;
    QLabel totalTime;
    bool isRunning = false;

    void recalculateTable();
    void updateTime();
    void reset();
};
