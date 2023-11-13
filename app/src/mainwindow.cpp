#include "mainwindow.h"

#include "qmidimessage.h"
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QSplitter>
#include <QToolBar>
#include <QWidgetAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    this->setMinimumSize(1200, 750);
    this->createComponents();
    this->createConnections();

    //    connect(m_button, &QPushButton::clicked, this, [this]() {
    //        QList<QScreen *> screens = QGuiApplication::screens();
    //        if (screens.size() > 1) {
    //            const QRect secondScreenGeometry = screens.at(1)->availableGeometry();
    //            QRect frameGeometry = this->frameGeometry();
    //            int titleBarHeight = frameGeometry.height() - this->geometry().height();

    //            int x = secondScreenGeometry.x() + secondScreenGeometry.width() / 2 - this->geometry().width() / 2;
    //            int y = secondScreenGeometry.y() + secondScreenGeometry.height() / 2 - this->geometry().height() / 2;

    //            qDebug() << "secondScreenGeometry: " << QPoint(x, y);
    //            this->setGeometry(x, y, this->width(), this->height());
    //        }
    //    });
}

MainWindow::~MainWindow() {
}

void MainWindow::createComponents() {
    m_splitter = new QSplitter(this);
    this->setCentralWidget(m_splitter);

    m_textEdit = new QTextEdit();
    m_textEdit->setFixedWidth(500);
    m_rightWidget = new WidgetBase();
    m_splitter->addWidget(m_textEdit);

    m_bottomWidget = new WidgetBase();

    auto container = new QWidget(this);
    QVBoxLayout *v = new QVBoxLayout(container);
    v->addWidget(m_rightWidget);
    v->addWidget(m_bottomWidget);

    m_splitter->addWidget(container);

    QHBoxLayout *h = new QHBoxLayout(m_rightWidget);

    for (int i = 0; i < 9; ++i) {
        auto t = new TinyGroup(this);
        h->addWidget(t);
        m_group.append(t);
    }

    QGridLayout *g = new QGridLayout(m_bottomWidget);
    g->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_statusButton->setMaximumSize(80, 30);
    m_statusButton->setStyleSheet("QPushButton:pressed { background-color: red; }");
    m_setButton->setMaximumSize(80, 30);
    m_clearButton->setMaximumSize(80, 30);
    g->addWidget(m_statusButton, 0, 0);
    g->addWidget(m_setButton, 1, 0);
    g->addWidget(m_statusLabel, 1, 1, 1, 3);
    connect(m_setButton, &QPushButton::clicked, this, [this]() {
        m_statusLabel->setText("Start Record");
        m_recordMessage.clear();
    });
    connect(m_clearButton, &QPushButton::clicked, this, [this]() {
        m_statusLabel->setText("Cleared");
        m_recordMessage.clear();
    });
    g->addWidget(m_clearButton, 2, 0);
}

void MainWindow::createConnections() {
    QObject::connect(m_textEdit->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this]() {
        // Scroll to the bottom whenever the range changes (e.g., new text is added)
        m_textEdit->verticalScrollBar()->setValue(m_textEdit->verticalScrollBar()->maximum());
    });

    m_midi.listen();

    QObject::connect(&m_midi, &QMidiIn::midiMessageReceived, this, [this](QMidiMessage *message) {
        QString s;
        QTextStream d(&s, QIODeviceBase::ReadWrite);
        unsigned int nBytes = 3;
        if (nBytes > 0) {
            d << "Timestamp: " << message->getDeltaTime() << ", Status: " << QString("%1").arg(message->getStatus(), 2, 16, QChar('0')) << "(int:" << message->getStatus() << ")"
              << ", Data1: " << QString("%1").arg(message->getRawMessage().at(1), 2, 16, QChar('0')) << "(int:" << message->getRawMessage().at(1) << ")"
              << ", Data2: " << QString("%1").arg(message->getRawMessage().at(2), 2, 16, QChar('0')) << "(int:" << message->getRawMessage().at(2) << ")"
              << ", Channel: " << QString("%1").arg(message->getChannel(), 2, 16, QChar('0')) << "(int:" << message->getChannel() << ")";
        }

        m_textEdit->append(s);
        qDebug().noquote() << s;

        switch (message->getStatus()) {
            case MIDI_NOTE_ON:
            case MIDI_NOTE_OFF: { // 按钮 0x90==144
                if (m_buttonMap.contains(message->getPitch())) {
                    auto button = this->getButtonByIndex(m_buttonMap[message->getPitch()]);
                    qDebug().noquote() << m_buttonMap[message->getPitch()] << ", " << button;
                    button->setDown(message->getVelocity() != 0);
                }
            } break;
            case MIDI_CONTROL_CHANGE: { // 顶部旋钮 0xB0==176
                if (m_labelMap.contains(message->getControl())) {
                    auto label = this->getLabelByIndex(m_labelMap[message->getControl()]);
                    qDebug().noquote() << "message->getControl(): " << message->getControl() << ", " << m_labelMap[message->getControl()] << ", " << label;

                    if (label) {
                        if (m_labelMap[message->getControl()] == 9) {
                            label->setText(QString::number(message->getValue()));
                        } else {
                            int v = label->text().toInt();
                            switch (message->getValue()) {
                                case 1: { // ++
                                    ++v;
                                    if (v > 127) {
                                        v = 127;
                                    }
                                } break;
                                case 65: { // --
                                    --v;
                                    if (v < 0) {
                                        v = 0;
                                    }
                                } break;
                                case 127: { // 0
                                    v = 0;
                                } break;
                                case 63: { // 127
                                    v = 127;
                                } break;
                            }

                            label->setText(QString::number(v));
                        }
                    } else {
                        qFatal() << "label == nullptr";
                    }
                }
            } break;
            case MIDI_PITCH_BEND: { // 推杆 0xE0==224
                if (m_sliderMap.contains(message->getChannel())) {
                    auto slider = this->getSliderByIndex(m_sliderMap[message->getChannel()]);
                    if (slider) {
                        qDebug().noquote() << "MIDI_PITCH_BEND: " << m_sliderMap[message->getChannel() - 1] << ", " << slider << ", " << message->getValue();
                        slider->setValue(message->getValue());
                    } else {
                        qFatal() << "slider == nullptr";
                    }
                }
            } break;
            default: {
                qDebug().noquote() << "uncatch Status, " << message->getStatus();
            } break;
        }

        if (!m_recordMessage.isEmpty() && m_recordMessage.getStatus() == message->getStatus() && m_recordMessage.getPitch() == message->getPitch()) {
            m_statusButton->setDown(m_recordMessage.getVelocity() == message->getVelocity());
        }

        if (m_statusLabel->text() == "Start Record" && m_recordMessage.isEmpty()) {
            qDebug() << "Start Record";
            m_recordMessage = *message;
            qDebug() << "Start Record 2";
            m_statusLabel->setText([this]() {
                return QString("%1, %2, %3").arg(QString::number(m_recordMessage.getStatus()), QString::number(m_recordMessage.getPitch()), QString::number(m_recordMessage.getVelocity()));
            }());
        }

        delete message;
    });
}

QPushButton *MainWindow::getButtonByIndex(int index) {
    QPushButton *button = nullptr;
    if (index <= 9) {
        button = m_group[index - 1]->getTopButton();
    } else if (index > 9) {
        button = m_group[index - 9 - 1]->getBottomButton();
    }
    return button;
}

QLabel *MainWindow::getLabelByIndex(int index) {
    QLabel *label = nullptr;
    qDebug() << "index - 1: " << index - 1;
    qDebug() << "m_group.count(): " << m_group.count();
    if ((index - 1) >= 0 && (index - 1) < m_group.count()) {
        label = m_group[index - 1]->getLabel();
    } else {
        qFatal() << "getLabelByIndex, index not valid";
    }
    return label;
}

QSlider *MainWindow::getSliderByIndex(int index) {
    QSlider *slider = nullptr;
    if ((index - 1) >= 0 && (index - 1) < m_group.count()) {
        slider = m_group[index - 1]->getSlider();
    } else {
        qFatal() << "getSliderByIndex, index not valid";
    }
    return slider;
}
