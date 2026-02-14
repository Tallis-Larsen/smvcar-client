#include "../include/LapStopwatch.h"

LapStopwatch::LapStopwatch(QWidget* parent) : QWidget(parent), table(this), updateTimer(this), totalTime("00:00.00", this) {
    setGeometry(0, 0, 480, 320);

    table.setColumnCount(3);
    table.verticalHeader()->hide();
    table.setHorizontalHeaderLabels({"Lap #", "Time", "Timestamp", "command_id"});
    table.hideColumn(2);
    table.hideColumn(3);
    table.setGeometry(0, 25, 200, 295);

    totalTime.setGeometry(0, 0, 200, 25);

    // Disables some unneeded UI elements
    table.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.setSelectionMode(QAbstractItemView::NoSelection);

    // Removes the initially highlighted cell at startup
    table.setCurrentItem(nullptr);

    connect(&updateTimer, &QTimer::timeout, this, &LapStopwatch::updateTime);
    connect(&ServerAPI::instance(), &ServerAPI::addLap, this, &LapStopwatch::addRow);
    connect(&ServerAPI::instance(), &ServerAPI::removeLap, this, &LapStopwatch::removeRow);
    connect(&ServerAPI::instance(), &ServerAPI::addStopwatchStart, this, &LapStopwatch::start);
    connect(&ServerAPI::instance(), &ServerAPI::addStopwatchStop, this, &LapStopwatch::stop);
}

void LapStopwatch::startStopReset() {
    if (!isRunning && table.rowCount() == 0) {
        start();
    } else if (isRunning) {
        stop();
    } else {
        reset();
    }
}

void LapStopwatch::start() {
    updateTimer.start(50);
    isRunning = true;
    QString commandId = ServerAPI::instance().startStopwatch(QDateTime::currentDateTime());
    addRow(QDateTime::currentDateTime(), commandId);
    updateTime();
}

void LapStopwatch::stop() {
    updateTimer.stop();
    isRunning = false;
    ServerAPI::instance().stopStopwatch(QDateTime::currentDateTime());
}

void LapStopwatch::reset() {
    table.setRowCount(0);
    totalTime.setText("00:00.00");
    ServerAPI::instance().resetStopwatch();
}

void LapStopwatch::lap() {
    if (isRunning) {
        QString commandId = ServerAPI::instance().lap(QDateTime::currentDateTime());
        addRow(QDateTime::currentDateTime(), commandId);
    }
}

void LapStopwatch::addRow(const QDateTime& lapTime, const QString& commandId) {
    table.insertRow(0);

    // Storing the time the lap was recorded
    QTableWidgetItem* timeItem = new QTableWidgetItem(lapTime.toString(Qt::ISODateWithMs));
    table.setItem(0, 2, timeItem);

    QTableWidgetItem* idItem = new QTableWidgetItem(commandId);
    table.setItem(0, 3, idItem);

    recalculateTable();
}

void LapStopwatch::removeRow(const QString& commandId) {

    for (int i = 0; i < table.rowCount(); i++) {
        if (table.item(i, 3)->text() == commandId) {
            table.removeRow(i);
            break;
        }
    }

    recalculateTable();
}

void LapStopwatch::recalculateTable() {
    // We can't guarantee that the row added will be in order, so this sorts it
    table.sortByColumn(2, Qt::AscendingOrder);

    // Recalculate row numbers
    for (int i = 0; i < table.rowCount(); i++) {
        QTableWidgetItem* rowNumber = new QTableWidgetItem(QString::number(i + 1));
        rowNumber->setTextAlignment(Qt::AlignCenter);
        table.setItem(i, 0, rowNumber);
    }

    // Calculating the timers for all inactive rows
    for (int i = 1; i < table.rowCount(); i++) {
        QTableWidgetItem* previousItem = table.item(i - 1, 2);
        QTableWidgetItem* currentItem = table.item(i, 2);

        QDateTime previousTimestamp = QDateTime::fromString(previousItem->text(), Qt::ISODateWithMs);
        QDateTime currentTimestamp = QDateTime::fromString(previousItem->text(), Qt::ISODateWithMs);

        centiseconds timeElapsed = std::chrono::floor<centiseconds>(std::chrono::milliseconds{currentTimestamp.msecsTo(previousTimestamp)});
        QTableWidgetItem* newItem = new QTableWidgetItem(QString::fromStdString(std::format("{:%M:%S}", timeElapsed)));
        table.setItem(i, 1, newItem);
    }
}

void LapStopwatch::updateTime() {
    if (table.rowCount() == 0) { return; }

    // Setting the totalTime based on the time since the stopwatch started
    QTableWidgetItem* startTimeItem = table.item(table.rowCount() - 1, 2);
    QDateTime startTime = QDateTime::fromString(startTimeItem->text(), Qt::ISODateWithMs);
    centiseconds totalDuration = std::chrono::floor<centiseconds>
        (std::chrono::milliseconds{startTime.msecsTo(QDateTime::currentDateTime())});
    totalTime.setText(QString::fromStdString(std::format("{:%M:%S}", totalDuration)));

    // Now we update the active lap the same way
    QTableWidgetItem* lapStartItem = table.item(0, 2);
    QDateTime lapStartTime = QDateTime::fromString(lapStartItem->text(), Qt::ISODateWithMs);
    centiseconds lapDuration = std::chrono::floor<centiseconds>
        (std::chrono::milliseconds{lapStartTime.msecsTo(QDateTime::currentDateTime())});
    QTableWidgetItem* newItem = new QTableWidgetItem(QString::fromStdString(std::format("{:%M:%S}", lapDuration)));
    table.setItem(0, 1, newItem);
}
