#include "../include/ServerDialogue.h"

ServerDialogue::ServerDialogue(QWidget* parent) : QDialog(parent), label("Connect to Server", this),
    urlField(this), cancelButton("Cancel", this), connectButton("Connect", this) {

    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addWidget(&connectButton);

    mainLayout.addWidget(&label);
    mainLayout.addWidget(&urlField);
    mainLayout.addLayout(&buttonLayout);

    connect(&cancelButton, &QPushButton::clicked, this, &QDialog::hide);
    connect(&connectButton, &QPushButton::clicked, this, &ServerDialogue::connectButtonPressed);

    setLayout(&mainLayout);
    setGeometry(25, 25, 430, 270);

    urlField.setText(ServerAPI::instance().getUrl());
}

void ServerDialogue::connectButtonPressed() {
    ServerAPI::instance().setUrl(urlField.text());
    hide();
}

void ServerDialogue::keyPressEvent(QKeyEvent* event) {

    // Fixes ESC to exit dialogue
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        hide();
        return;
    }

    QDialog::keyPressEvent(event);
}