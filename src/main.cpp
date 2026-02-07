#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    // Create an application object. Every Qt program needs one.
    QApplication application(argc, argv);
    application.setOverrideCursor(Qt::BlankCursor); // Hide the cursor

    MainWindow window = MainWindow();
    window.start();

    // Start the event loop
    return application.exec();
}