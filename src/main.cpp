#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    // Create an application object. Every Qt program needs one.
    QApplication application(argc, argv);

    QWidget window;
    QPalette palette = window.palette();
    palette.setColor(QPalette::Window, QColor(0, 0, 0));
    window.resize(480, 320);
    window.setPalette(palette);
    window.setAutoFillBackground(true);
    window.show();

    // Start the event loop
    return application.exec();
}
