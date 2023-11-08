
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QShortcut>
#include <QObject>
#include <QProxyStyle>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTime>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QTextStream>
#include "mainwindow.h"

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    static QMutex mutex;
    static QFile file;

    mutex.lock();
    if (!file.isOpen()) {
        QString logPath{"."};
        // = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append(QString::fromLocal8Bit(""));


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
    switch (type)
    {
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
    //file.close();
    mutex.unlock();
}

class NoFocusProxyStyle : public QProxyStyle {
public:

    NoFocusProxyStyle(QStyle *baseStyle = 0) : QProxyStyle(baseStyle) {}

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const {
        if(element == QStyle::PE_FrameFocusRect) {
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    qSetMessagePattern("File: %{file}: Line %{line}: \n>> %{message}\n");

    if (!qgetenv("QTDIR").isEmpty()) { // start direct in QtCreator
        qDebug() << "start in QtCreator: " << qgetenv("QTDIR");
    } else if (!qgetenv("VSAPPIDDIR").isEmpty()) { // start direct in vs
        qDebug() << "start in vs: " << qgetenv("VSAPPIDDIR");
    } else {
        qInstallMessageHandler(outputMessage);
    }

    app.setStyle(new NoFocusProxyStyle);

    /*
        QTableView {
            outline: 0;
            border: none;
        }
        QTableView::item { border: none; }
    */
    app.setStyleSheet(R"(
        QMenuBar {
            background-color: gray;
            color: white;
        }
        QMenuBar::hover {
            background-color: rgba(128,128,128,1);
            color: yellow;
        }
        QMenuBar::selected {
            background-color: rgba(255,0,0,1);
            color: yellow;
        }
        QMenuBar::item {
            font-size:12px;
            font-family:Microsoft YaHei;
            background-color: rgba(60,60,60,1);
            color:rgba(255,255,255,1);
        }
        QMenu {
            background-color:rgba(17,24,47,1);
            border:1px solid rgba(70, 70, 70, 1);
        }
        QMenu::item {
            min-width:50px;
            font-size: 12px;
            color: rgb(225,225,225);
            background:rgba(75,120,154,0.5);
            border:1px solid rgba(70, 70, 70, 1);
            padding:1px 1px;margin:1px 1px;
        }
        QMenu::item:selected {
            background:rgba(70, 70, 70, 1);
            border:1px solid rgba(70, 70, 70, 1);
        }
        QMenu::item:pressed {
            background:rgba(70, 70, 70, 0.4);
            border:1px solid rgba(70, 70, 70, 1);
        }
    )");

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

    app.setProperty("RandomColor", false);

    try {
        qDebug() << "Version: " << RtMidi::getVersion();
        RtMidiIn midiin;
    } catch (RtMidiError &error) {
        // Handle the exception here
        error.printMessage();
    }

    return app.exec();
}
