#include "../include/WiFiDialogue.h"

WiFiDialogue::WiFiDialogue(QWidget* parent) : QDialog(parent), label("Connect to Wi-Fi", this),
    refreshButton("Refresh", this), apList(this), passwordField(this),
    cancelButton("Cancel", this), connectButton("Connect", this) {

    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addWidget(&connectButton);

    mainLayout.addWidget(&label);
    mainLayout.addWidget(&refreshButton);
    mainLayout.addWidget(&apList);
    mainLayout.addWidget(&passwordField);
    mainLayout.addLayout(&buttonLayout);

    // Setting callbacks
    connect(&refreshButton, &QPushButton::clicked, &apList, &APList::refresh);
    connect(&cancelButton, &QPushButton::clicked, this, &QDialog::hide);
    connect(&connectButton, &QPushButton::clicked, this, &WiFiDialogue::connectButtonPressed);

    setLayout(&mainLayout);
    setGeometry(25, 25, 430, 270);
}

void WiFiDialogue::keyPressEvent(QKeyEvent* event) {

    // Fixes ESC to exit dialogue
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        hide();
        return;
    }

    QDialog::keyPressEvent(event);
}

void WiFiDialogue::connectButtonPressed() {
    hide();
}
