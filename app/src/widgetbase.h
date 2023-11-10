#pragma once

#include <QWidget>
#include <QRandomGenerator>
#include <QPainter>
#include <QApplication>
#include <QPainterPath>

class WidgetBase : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetBase(QWidget* parent = nullptr);
    virtual ~WidgetBase() {
    }

    virtual QString realClassName() {
        return this->metaObject()->className();
    }

protected:
    QColor randomColor = QColor::fromRgb(QRandomGenerator::global()->generate());
    void paintEvent(QPaintEvent* event) override {
        if (qApp->property("RandomColor").toBool()) {
            QPainter painter(this);
            QPen pen;
            pen.setWidth(5);
            painter.fillRect(rect(), randomColor);
            painter.setPen(pen);
            painter.drawRect(this->rect());
            // painter.drawText(0, 0, 120, 20, 0, this->realClassName());

            auto text = this->realClassName();

            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
            painter.setRenderHints(QPainter::TextAntialiasing, true);

            QFontMetricsF fm(painter.font());
            QPainterPath path;

            QPointF point(5, fm.height() / 2 + 5);
            if(realClassName() != "WidgetBase"){
                point = QPointF(5, fm.height() / 2 + fm.height() + 5);
            }

            path.addText(point, painter.font(), text);

            pen = painter.pen();
            pen.setWidth(2);
            pen.setColor(QColor(0, 0, 0));
            painter.setPen(pen);
            painter.setBrush(QColor(0, 0, 0));
            painter.drawPath(path);

            pen.setWidth(1);
            pen.setColor(QColor(255, 255, 255));
            painter.setPen(pen);
            painter.setBrush(QColor(255, 255, 255));
            painter.drawText(point, text);
        }
    }

signals:

private:
};
