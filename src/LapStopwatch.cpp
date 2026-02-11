#include "../include/LapStopwatch.h"

LapStopwatch::LapStopwatch(QWidget* parent) : QWidget(parent), table(this), updateTimer(this), totalTime("00:00.00", this) {
    setGeometry(0, 0, 480, 320);

    table.setColumnCount(3);
    table.verticalHeader()->hide();
    table.setHorizontalHeaderLabels({"Lap Number", "Delta Time", "Total Time"});
    table.setGeometry(0, 25, 300, 295);

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

void LapStopwatch::startStop() {
    if (!isRunning) {
        timer.start();
        updateTimer.start(50);
        isRunning = true;
        addRow();
    }
}

void LapStopwatch::lap() {
    if (isRunning) {
        addRow();
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
    std::chrono::nanoseconds duration = timer.durationElapsed();

    // Creating a centiseconds type set to 1/100th of a second
    using centiseconds = std::chrono::duration<int, std::ratio<1, 100>>;

    // Rounding down from nanoseconds to centiseconds
    centiseconds roundedDuration = std::chrono::floor<centiseconds>(duration);

    // Converting to formatted string
    QString durationString = QString::fromStdString(std::format("{:%M:%S}", roundedDuration));

    totalTime.setText(durationString);

    // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
    QTableWidgetItem* durationItem = new QTableWidgetItem(durationString);

    // Because we are 'giving' durationItem to the QTableWidget, Qt will now handle memory management for it.
    table.setItem(0, 2, durationItem);
}
