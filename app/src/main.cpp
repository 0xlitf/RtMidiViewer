
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QMutex>
#include <QObject>
#include <QProxyStyle>
#include <QShortcut>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTime>
#include <QTranslator>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <vector>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static QMutex mutex;
    static QFile file;

    mutex.lock();
    if (!file.isOpen()) {
        QString logPath{"."}; // = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append(QString::fromLocal8Bit(""));

        QDir logDir(logPath);

        if (!logDir.exists()) {
            logDir.mkpath(logPath);
        }

        file.setFileName(logPath + QString("/app_%1.log").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")));
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        //text_stream.setDevice(&file);
        //text_stream.setCodec("UTF-8");
        //text_stream << "\r\n" << "\r\n";
        //file.flush();
    }

    QString text;
    switch (type) {
        case QtDebugMsg:
            text = QString("Debug");
            break;
        case QtWarningMsg:
            text = QString("Warning");
            break;
        case QtCriticalMsg:
            text = QString("Critical");
            break;
        case QtFatalMsg:
            text = QString("Fatal");
        case QtInfoMsg:;
            text = QString("Info");
    }

    QString message = QString("[%1 File:%2,Line:%3 %4]    %5")
                          .arg(text)
                          .arg(QString::fromLocal8Bit(context.file))
                          .arg(QString::number(context.line))
                          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                          .arg(msg);

    QTextStream text_stream(&file);
    // text_stream.setCodec("UTF-8");
    text_stream << message << "\r\n";
    file.flush();
    mutex.unlock();
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);

    QApplication app(argc, argv);
    app.setProperty("RandomColor", false);

    if (!qgetenv("QTDIR").isEmpty()) { // start direct in QtCreator
        qSetMessagePattern("File: %{file}: Line %{line}: \n>> %{message}\n");
        qDebug() << "start in QtCreator: " << qgetenv("QTDIR");
    } else if (!qgetenv("VSAPPIDDIR").isEmpty()) { // start direct in vs
        qSetMessagePattern("File: %{file}: Line %{line}: \n>> %{message}\n");
        qDebug() << "start in vs: " << qgetenv("VSAPPIDDIR");
    } else {
        qInstallMessageHandler(outputMessage);
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "i18n_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
