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
    this->setMinimumSize(1000, 750);
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
    m_textEdit->setFixedWidth(400);
    m_rightWidget = new WidgetBase();
    m_splitter->addWidget(m_textEdit);
    m_splitter->addWidget(m_rightWidget);

    QObject::connect(m_textEdit->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this]() {
        // Scroll to the bottom whenever the range changes (e.g., new text is added)
        m_textEdit->verticalScrollBar()->setValue(m_textEdit->verticalScrollBar()->maximum());
    });
}

void MainWindow::createConnections() {
    m_midi.listen();

    QObject::connect(&QRtMidiWrapper::getInstance(), &QRtMidiWrapper::dataReceived, &QRtMidiWrapper::getInstance(), [this](double deltatime, QList<int> message, void *userData) {
        QString s;
        QTextStream d(&s, QIODeviceBase::ReadWrite);
        unsigned int nBytes = message.size();
        if (nBytes > 0) {
            d << "Timestamp: " << deltatime << ", Status: " << QString("%1").arg(message.at(0), 2, 16, QChar('0')) << "(int:" << message.at(0) << ")"
              << ", Data1: " << QString("%1").arg(message.at(1), 2, 16, QChar('0')) << "(int:" << message.at(1) << ")"
              << ", Data2: " << QString("%1").arg(message.at(2), 2, 16, QChar('0')) << "(int:" << message.at(2) << ")";
        }

        m_textEdit->append(s);
        qDebug().noquote() << s;
    });
}
