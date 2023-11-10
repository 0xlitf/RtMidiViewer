#pragma once

#include <QMainWindow>
#include <QAction>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QStyleOption>
#include <QRandomGenerator>
#include <QApplication>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <RtMidi.h>
#include "qrtmidiwrapper.h"
#include "tinygroup.h"
#include "widgetbase.h"

class RoundWidget: public QWidget {
public:
    RoundWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()): QWidget(parent, f){

    }
    virtual QString realClassName() {
        return this->metaObject()->className();
    }

protected:
    void paintEvent(QPaintEvent* event){
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

        QBitmap bmp(this->size());
        bmp.fill();
        QPainter painter(&bmp);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawRoundedRect(bmp.rect(), 5, 5);
        setMask(bmp);
        painter.drawText(0, 0, 120, 20, 0, this->realClassName());

    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    virtual QString realClassName() {
        return this->metaObject()->className();
    }

private:
    void createComponents();
    void createConnections();


protected:
    QColor randomColor = QColor::fromRgb(QRandomGenerator::global()->generate());
    void paintEvent(QPaintEvent* event) override {
        if (qApp->property("RandomColor").toBool()) {
            QPainter painter(this);
            painter.fillRect(rect(), randomColor);
            painter.drawText(0, 0, 120, 20, 0, this->realClassName());
        }

        QMainWindow::paintEvent(event);
    }

private:
    // QPushButton* m_button = new QPushButton(this);
    QRtMidiWrapper m_midi;
    QTextEdit* m_textEdit = nullptr;
    WidgetBase* m_rightWidget = nullptr;
    QSplitter *m_splitter = nullptr;
};
