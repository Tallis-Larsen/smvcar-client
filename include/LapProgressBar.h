#pragma once
#include <QProgressBar>
#include <QPainter>

class LapProgressBar : public QProgressBar {
    Q_OBJECT
public:
    LapProgressBar(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* event) override;
};