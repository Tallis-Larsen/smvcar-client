#include "../include/LapStopwatch.h"

LapStopwatch::LapStopwatch(QWidget* parent) : QWidget(parent), table(this), updateTimer(this), totalTime("00:00.00", this) {
    setGeometry(0, 0, 480, 320);

    table.setColumnCount(3);
    table.verticalHeader()->hide();
    table.setHorizontalHeaderLabels({"Lap #", "Time", "Raw Lap Time (centiseconds)"});
    table.hideColumn(2);
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

}

void LapStopwatch::startStopReset() {
    if (!isRunning && table.rowCount() == 0) {
        // Start
        timer.start();
        updateTimer.start(50);
        isRunning = true;
        addRow();
        lastLapTime = std::chrono::nanoseconds{0};
    } else if (isRunning) {
        // Stop
        timer.invalidate();
        updateTimer.stop();
        isRunning = false;
    } else {
        // Reset
        table.setRowCount(0);
        totalTime.setText("00:00.00");
    }
}

void LapStopwatch::lap() {
    if (isRunning) {
        addRow();
        lastLapTime = timer.durationElapsed();
    }
}

void LapStopwatch::addRow() {
    table.insertRow(0);
    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QTableWidgetItem* rowNumber = new QTableWidgetItem(QString::number(table.rowCount()));
    rowNumber->setTextAlignment(Qt::AlignCenter);
    table.setItem(0, 0, rowNumber);
}

void LapStopwatch::updateTime() {
    std::chrono::nanoseconds totalDuration = timer.durationElapsed();
    std::chrono::nanoseconds lapDuration = totalDuration - lastLapTime;

    // Creating a centiseconds type set to 1/100th of a second
    using centiseconds = std::chrono::duration<int, std::ratio<1, 100>>;

    // Rounding down from nanoseconds to centiseconds
    centiseconds roundedDuration = std::chrono::floor<centiseconds>(totalDuration);
    // Converting to formatted string
    QString durationString = QString::fromStdString(std::format("{:%M:%S}", roundedDuration));
    // Setting to totalTime label
    totalTime.setText(durationString);

    // Re-using the same variables and process for the lapDuration
    roundedDuration = std::chrono::floor<centiseconds>(lapDuration);
    durationString = QString::fromStdString(std::format("{:%M:%S}", roundedDuration));
    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QTableWidgetItem* durationItem = new QTableWidgetItem(durationString);
    // We want to save the duration as an integer still for sending to the server
    QTableWidgetItem* rawDurationItem = new QTableWidgetItem(QString::number(roundedDuration.count()));

    // Because we are 'giving' durationItem to the QTableWidget, Qt will now handle memory management for it.
    table.setItem(0, 1, durationItem);
    table.setItem(0, 2, rawDurationItem);
}
