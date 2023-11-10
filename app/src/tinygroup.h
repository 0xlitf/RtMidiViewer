#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>

class TinyGroup : public QWidget
{
    Q_OBJECT
public:
    explicit TinyGroup(QWidget *parent = nullptr);
    void createComponents();
    QLabel* getLabel() {
        return m_label;
    }
    QSlider* getSlider() {
        return m_slider;
    }
    QPushButton* getTopButton() {
        return m_button1;
    }
    QPushButton* getBottomButton() {
        return m_button2;
    }
signals:

private:
    QLabel* m_label = new QLabel(this);
    QPushButton* m_button1 = new QPushButton(this);
    QPushButton* m_button2 = new QPushButton(this);
    QSlider* m_slider = new QSlider(this);
};

