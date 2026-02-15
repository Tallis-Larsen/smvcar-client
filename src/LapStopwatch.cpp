#include "../include/LapStopwatch.h"

LapStopwatch::LapStopwatch(QWidget* parent) : QWidget(parent), table(this), updateTimer(this),
    totalTime("00:00.00", this), lapProgress(this), timeProgress(this),
    currentLapDisplay("Laps: 0", this), targetLapsDisplay("Target: 0", this), targetTimeDisplay("00:00", this) {

    setGeometry(0, 0, 480, 320);

    table.setColumnCount(4);
    table.verticalHeader()->hide();
    table.setHorizontalHeaderLabels({"Lap", "Time", "Timestamp", "command_id"});
    table.hideColumn(2);
    table.hideColumn(3);
    table.setGeometry(0, 50, 280, 270);
    table.verticalHeader()->setDefaultSectionSize(35);
    table.setColumnWidth(0, 100);
    table.setColumnWidth(1, 180);

    totalTime.setGeometry(0, 0, 200, 25);
    timeProgress.setGeometry(75, 0, 330, 25);
    timeProgress.setMinimum(0);
    targetTimeDisplay.setGeometry(405, 0, 75, 25);
    targetTimeDisplay.setAlignment(Qt::AlignRight);

    currentLapDisplay.setGeometry(0, 25, 75, 25);
    lapProgress.setGeometry(75, 25, 330, 25);
    lapProgress.setMinimum(0);
    targetLapsDisplay.setGeometry(405, 25, 75, 25);
    targetLapsDisplay.setAlignment(Qt::AlignRight);

    QPalette palette = timeProgress.palette();
    palette.setColor(QPalette::Highlight, QColor("#FFFFFF"));
    timeProgress.setPalette(palette);
    lapProgress.setPalette(palette);

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
    connect(&ServerAPI::instance(), &ServerAPI::setTargetTime, this, &LapStopwatch::setTargetTime);
    connect(&ServerAPI::instance(), &ServerAPI::setTargetLaps, this, &LapStopwatch::setTargetLaps);
}

void LapStopwatch::startStopReset() {
    if (!isRunning && table.rowCount() == 0) {
        QString commandId = ServerAPI::instance().startStopwatch(QDateTime::currentDateTime());
        QDateTime time = QDateTime::currentDateTime();
        start(time, commandId);
    } else if (isRunning) {
        stop();
        ServerAPI::instance().stopStopwatch(QDateTime::currentDateTime());
    } else {
        reset();
    }
}

void LapStopwatch::start(QDateTime time, QString id) {
    updateTimer.start(50);
    isRunning = true;
    addRow(time, id);
}

void LapStopwatch::stop() {
    updateTimer.stop();
    updateTime(); // One last update
    isRunning = false;
}

void LapStopwatch::reset() {
    table.setRowCount(0);
    lapProgress.setValue(0);
    timeProgress.setValue(0);
    currentLapDisplay.setText("Laps: 0");
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
    QTableWidgetItem* timeItem = new QTableWidgetItem(lapTime.toUTC().toString(Qt::ISODateWithMs));
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

void LapStopwatch::setTargetTime(std::chrono::minutes minutes) {
    timeProgress.setMaximum(std::chrono::seconds(minutes).count());
    targetTimeDisplay.setText(QString::fromStdString(std::format("{:%M:%S}", minutes)));
}

void LapStopwatch::setTargetLaps(int laps) {
    lapProgress.setMaximum(laps);
    targetLapsDisplay.setText("Target: " + QString::number(laps));
}

void LapStopwatch::recalculateTable() {
    // We can't guarantee that the row added will be in order, so this sorts it
    table.sortByColumn(2, Qt::DescendingOrder);

    // Recalculate row numbers
    for (int i = table.rowCount() - 1; i > -1; i--) {
        QTableWidgetItem* rowNumber = new QTableWidgetItem(QString::number(table.rowCount() - i));
        rowNumber->setTextAlignment(Qt::AlignCenter);
        table.setItem(i, 0, rowNumber);
    }

    // Calculating the timers for all inactive rows
    for (int i = 1; i < table.rowCount(); i++) {
        QTableWidgetItem* previousItem = table.item(i - 1, 2);
        QTableWidgetItem* currentItem = table.item(i, 2);

        QDateTime previousTimestamp = QDateTime::fromString(previousItem->text(), Qt::ISODateWithMs);
        QDateTime currentTimestamp = QDateTime::fromString(currentItem->text(), Qt::ISODateWithMs);

        centiseconds timeElapsed = std::chrono::floor<centiseconds>(std::chrono::milliseconds{currentTimestamp.msecsTo(previousTimestamp)});
        QTableWidgetItem* newItem = new QTableWidgetItem(QString::fromStdString(std::format("{:%M:%S}", timeElapsed)));

        updateColors(newItem, std::chrono::seconds{currentTimestamp.msecsTo(previousTimestamp)});

        table.setItem(i, 1, newItem);
    }

    if (table.rowCount() < 1) { return; }

    lapProgress.setValue(table.rowCount() - 1);
    currentLapDisplay.setText("Laps: " + QString::number(table.rowCount() - 1));

}

void LapStopwatch::updateTime() {
    if (table.rowCount() == 0) { return; }

    // Setting the totalTime based on the time since the stopwatch started
    QTableWidgetItem* startTimeItem = table.item(table.rowCount() - 1, 2);
    QDateTime startTime = QDateTime::fromString(startTimeItem->text(), Qt::ISODateWithMs);
    centiseconds totalDuration = std::chrono::floor<centiseconds>
        (std::chrono::milliseconds{startTime.msecsTo(QDateTime::currentDateTime())});
    totalTime.setText(QString::fromStdString(std::format("{:%M:%S}", totalDuration)));

    // Use the same variable to track the progress bar
    timeProgress.setValue(totalDuration.count() / 100);

    // Now we update the active lap the same way
    QTableWidgetItem* lapStartItem = table.item(0, 2);
    QDateTime lapStartTime = QDateTime::fromString(lapStartItem->text(), Qt::ISODateWithMs);
    centiseconds lapDuration = std::chrono::floor<centiseconds>
        (std::chrono::milliseconds{lapStartTime.msecsTo(QDateTime::currentDateTime())});
    QTableWidgetItem* newItem = new QTableWidgetItem(QString::fromStdString(std::format("{:%M:%S}", lapDuration)));

    updateColors(newItem, std::chrono::seconds{lapStartTime.msecsTo(QDateTime::currentDateTime())});

    table.setItem(0, 1, newItem);

}

void LapStopwatch::updateColors(QTableWidgetItem* item, std::chrono::seconds lapDuration) {
    if (lapDuration > targetLapTime) {
        item->setForeground(QColor("#FF0000")); // Red
    } else {
        item->setForeground(QColor("#00FF00")); // Green
    }
}
