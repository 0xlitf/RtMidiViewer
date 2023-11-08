#include "mainwindow.h"

#include <QPushButton>
#include <QToolBar>
#include <QMenuBar>
#include <QWidgetAction>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    this->setObjectName(this->metaObject()->className());
    this->setMinimumSize(1200, 750);

    this->createComponents();

    QScreen *scr = QApplication::primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    qDebug() << scr->size();
    qDebug() << this->size();
    qDebug() << this->frameGeometry();
    this->move((scr_w - this->frameGeometry().width()) / 2, (scr_h - this->frameGeometry().height()) / 2);

    qDebug() << this->geometry();
}

MainWindow::~MainWindow() {}

void MainWindow::createComponents() {

}
