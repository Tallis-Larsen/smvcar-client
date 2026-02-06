#include <QApplication>
#include <QTableWidget>
#include <QHeaderView>

int main(int argc, char *argv[])
{
    // Create an application object. Every Qt program needs one.
    QApplication application(argc, argv);

    QWidget window;
    QTableWidget table = QTableWidget(&window);
    QPalette palette = window.palette();
    palette.setColor(QPalette::Window, QColor(0, 0, 0));
    window.resize(480, 320);
    window.setPalette(palette);
    window.setAutoFillBackground(true);

    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.setSelectionMode(QAbstractItemView::NoSelection);
    table.setColumnCount(3);
    table.verticalHeader()->hide();
    table.setHorizontalHeaderLabels({"Lap Number", "Delta Time", "Total Time"});
    table.setGeometry(0, 25, 480, 295);
    table.setRowCount(20);
    for (int i = 0; i < 20; i++) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(i));
        item->setTextAlignment(Qt::AlignCenter);
        table.setItem(i, 0, item);
    }
    table.scrollToBottom();
    table.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Removes the intially highlighted cell at startup
    table.setCurrentItem(nullptr);

    table.show();
    window.show();

    // Start the event loop
    return application.exec();
}
