#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    // Create an application object. Every Qt program needs one.
    QApplication application(argc, argv);

    MainWindow window = MainWindow();

    // Start the event loop
    return application.exec();
}