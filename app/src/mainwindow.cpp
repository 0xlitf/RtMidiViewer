#include "mainwindow.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QToolBar>
#include <QScrollBar>
#include <QSplitter>
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
    connect(m_setButton, &QPushButton::clicked, this, [this](){
        m_statusLabel->setText("Start Record");
        m_recordMessage.clear();
    });
    connect(m_clearButton, &QPushButton::clicked, this, [this](){
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

    QObject::connect(&QRtMidiWrapper::getInstance(), &QRtMidiWrapper::dataReceived, &QRtMidiWrapper::getInstance(), [this](double deltatime, QList<int> message, void *userData) {
        QString s;
        QTextStream d(&s, QIODeviceBase::ReadWrite);
        unsigned int nBytes = message.size();
        if (nBytes > 0) {
            d << "Timestamp: " << deltatime << ", Status: " << QString("%1").arg(message.at(0), 2, 16, QChar('0')) << "(int:" << message.at(0) << ")"
              << ", Data1: " << QString("%1").arg(message.at(1), 2, 16, QChar('0')) << "(int:" << message.at(1) << ")"
              << ", Data2: " << QString("%1").arg(message.at(2), 2, 16, QChar('0')) << "(int:" << message.at(2) << ")"
              << ", Channel: " << QString("%1").arg((message.at(0) & 0x0F) + 1, 2, 16, QChar('0')) << "(int:" << (message.at(0) & 0x0F) + 1 << ")";
        }

        m_textEdit->append(s);
        qDebug().noquote() << s;

        if (message.at(0) >= 224 && message.at(0) <= 232) {
            if (m_sliderMap.contains(message.at(0))) {
                auto slider = this->getSliderByIndex(m_sliderMap[message.at(0)]);
                qDebug().noquote() << m_sliderMap[message.at(0)] << ", " << slider;
                slider->setValue(message.at(1));
            }
        } else {
            switch (message.at(0)) {
                case 144: {
                    if (m_buttonMap.contains(message.at(1))) {
                        auto button = this->getButtonByIndex(m_buttonMap[message.at(1)]);
                        qDebug().noquote() << m_buttonMap[message.at(1)] << ", " << button;
                        button->setDown(message.at(2) != 0);
                    }
                } break;
                case 176: {
                    if (m_labelMap.contains(message.at(1))) {
                        auto label = this->getLabelByIndex(m_labelMap[message.at(1)]);
                        qDebug().noquote() << m_labelMap[message.at(1)] << ", " << label;

                        if (m_labelMap[message.at(1)] == 9) {
                            label->setText(QString::number(message.at(2)));
                        } else {
                            int v = label->text().toInt();
                            switch (message.at(2)) {
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
                    }
                } break;
                default: {
                    qDebug().noquote() << "uncatch Status, " << message.at(0);
                } break;
            }
        }

        if (!m_recordMessage.isEmpty() && (m_recordMessage[0] == message[0]) && (m_recordMessage[1] == message[1])) {
            m_statusButton->setDown(m_recordMessage[2] == message[2]);
        }

        if (m_statusLabel->text() == "Start Record" && m_recordMessage.isEmpty()) {
            m_recordMessage = message;
            m_statusLabel->setText([this, &message]() {
                return QString("%1, %2, %3").arg(QString::number(message[0]), QString::number(message[1]), QString::number(message[2]));
            }());
        }
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

QLabel* MainWindow::getLabelByIndex(int index)
{
    QLabel *label = m_group[index - 1]->getLabel();
    return label;
}

QSlider* MainWindow::getSliderByIndex(int index)
{
    QSlider *slider = m_group[index - 1]->getSlider();
    return slider;
}
