#include <QGridLayout>
#include "tinygroup.h"
#include "qmidimessage.h"

TinyGroup::TinyGroup(QWidget *parent)
    : QWidget{parent}
{
    this->setFixedWidth(100);
    this->setFixedHeight(300);
    this->createComponents();
}

void TinyGroup::createComponents() {
    m_label->setAlignment(Qt::AlignCenter);
    QGridLayout* g = new QGridLayout(this);
    g->addWidget(m_label, 0, 0, 1, 2);
    g->addWidget(m_button1, 5, 0, 1, 1);
    g->addWidget(m_button2, 6, 0, 1, 1);
    g->addWidget(m_slider, 2, 1, 5, 2);
    m_slider->setMaximum(MIDI_MAX_BEND);
    m_slider->setMinimum(MIDI_MIN_BEND);

    m_button1->setStyleSheet("QPushButton:pressed { background-color: red; }");
    m_button2->setStyleSheet("QPushButton:pressed { background-color: green; }");
}
