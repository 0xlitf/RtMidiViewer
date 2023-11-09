#include "mainwindow.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QToolBar>
#include <QWidgetAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    this->setMinimumSize(1000, 750);
    this->createComponents();

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

}
