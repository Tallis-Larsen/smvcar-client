#include "../include/LapStopwatch.h"

LapStopwatch::LapStopwatch(QWidget* parent) : QTableWidget(parent) {
    setColumnCount(3);
    verticalHeader()->hide();
    setHorizontalHeaderLabels({"Lap Number", "Delta Time", "Total Time"});
    setGeometry(0, 25, 480, 295);
    setRowCount(20);

    for (int i = 0; i < 20; i++) {
        // ReSharper disable once CppDFAMemoryLeak | This does not actually leak because Qt handles the delete
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(i));
        item->setTextAlignment(Qt::AlignCenter);
        setItem(i, 0, item);
    }

    scrollToBottom();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    // Removes the initially highlighted cell at startup
    setCurrentItem(nullptr);
}
