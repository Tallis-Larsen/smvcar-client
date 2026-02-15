#include "../include/LapProgressBar.h"

LapProgressBar::LapProgressBar(QWidget* parent) : QProgressBar(parent) {

}

// AI wrote this thing, it just adds a "ghost" bar so you know where you'll be after the next lap.
void LapProgressBar::paintEvent(QPaintEvent* event) {
    QProgressBar::paintEvent(event);

    if (value() >= maximum()) { return; }

    qreal ghostFrac = static_cast<qreal>(value() + 1) / maximum();
    qreal fillFrac = static_cast<qreal>(value()) / maximum();

    QRect ghostRect = rect();
    ghostRect.setLeft(static_cast<int>(width() * fillFrac));
    ghostRect.setRight(static_cast<int>(width() * ghostFrac));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(0.4);
    painter.fillRect(ghostRect, QColor(150, 150, 150));

}
