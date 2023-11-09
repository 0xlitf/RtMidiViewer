
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
#include <iostream>
#include <cstdlib>
#include <vector>
#include <signal.h>
#include "mainwindow.h"
#include "qrtmidiwrapper.h"

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

void cstyle_rtmidi() {
    if (auto midiin = rtmidi_in_create_default()) {
        unsigned int ports = rtmidi_get_port_count(midiin);
        printf("MIDI input ports found: %u\n", ports);
        qDebug() << "MIDI input ports found: " << ports;
        for (int i = 0; i < ports; ++i) {
            char bufOut[256];
            int bufLen = sizeof(bufOut);
            int out = rtmidi_get_port_name(midiin, i, bufOut, &bufLen);
            qDebug() << QString("midiin.getPortName %1%2: %3").arg(i).arg(QString::fromLocal8Bit(bufOut));
        }
        rtmidi_close_port(midiin);
        rtmidi_in_free(midiin);
    }

    if (auto midiout = rtmidi_out_create_default()) {
        unsigned int ports = rtmidi_get_port_count(midiout);
        qDebug() << "MIDI output ports found: " << ports;
        for (int i = 0; i < ports; ++i) {
            char bufOut[256];
            int bufLen = sizeof(bufOut);
            int out = rtmidi_get_port_name(midiout, i, bufOut, &bufLen);
            qDebug() << QString("midiout.getPortName %1: %2").arg(i).arg(QString::fromLocal8Bit(bufOut));
        }
        rtmidi_close_port(midiout);
        rtmidi_out_free(midiout);
    }
}

void cppstyle_rtmidi() {
    try {
        qDebug() << "RtMidi::getVersion: " << RtMidi::getVersion();
        RtMidiIn midiin;
        for (int i = 0; i < midiin.getPortCount(); ++i) {
            qDebug() << "midiin.getPortName # " << i << ": " << midiin.getPortName(i);
        }
        RtMidiOut midiout;
        for (int i = 0; i < midiout.getPortCount(); ++i) {
            qDebug() << "midiout.getPortName # " << i << ": " << midiout.getPortName(i);
        }

    } catch (RtMidiError &error) {
        // Handle the exception here
        error.printMessage();
    }
}

void cpppointer_style_rtmidi() {
    RtMidiIn *midiin = nullptr;
    RtMidiOut *midiout = nullptr;

    // RtMidiIn constructor
    try {
        midiin = new RtMidiIn();
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    unsigned int nPorts = midiin->getPortCount();
    qDebug() << "There are " << nPorts << " MIDI input sources available.";
    std::string portName;
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            portName = midiin->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
            goto cleanup;
        }
        qDebug() << "  Input Port #" << i + 1 << ": " << portName;
    }

    // RtMidiOut constructor
    try {
        midiout = new RtMidiOut();
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    // Check outputs.
    nPorts = midiout->getPortCount();
    qDebug() << "There are " << nPorts << " MIDI output ports available.";
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            portName = midiout->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
            goto cleanup;
        }
        qDebug() << "  Output Port #" << i + 1 << ": " << portName;
    }

cleanup:
    delete midiin;
    delete midiout;
}

void mycallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    qDebug() << "mycallback";
    unsigned int nBytes = message->size();
    for (unsigned int i = 0; i < nBytes; i++)
        std::cout << "Byte " << i << " = " << (int) message->at(i) << ", ";
    if (nBytes > 0)
        std::cout << "stamp = " << deltatime << std::endl;
}

bool done;
static void finish( int /*ignore*/ ){ done = true; }

void midiin() {

}

int main(int argc, char *argv[]) {
    setbuf(stdout,NULL);

    QApplication app(argc, argv);

//    if (!qgetenv("QTDIR").isEmpty()) { // start direct in QtCreator
//        qDebug() << "start in QtCreator: " << qgetenv("QTDIR");
//    } else if (!qgetenv("VSAPPIDDIR").isEmpty()) { // start direct in vs
//        qDebug() << "start in vs: " << qgetenv("VSAPPIDDIR");
//    } else {
//        qSetMessagePattern("File: %{file}: Line %{line}: \n>> %{message}\n");
//        qInstallMessageHandler(outputMessage);
//    }

//    QTranslator translator;
//    const QStringList uiLanguages = QLocale::system().uiLanguages();
//    for (const QString &locale : uiLanguages) {
//        const QString baseName = "i18n_" + QLocale(locale).name();
//        if (translator.load(":/i18n/" + baseName)) {
//            app.installTranslator(&translator);
//            break;
//        }
//    }

    MainWindow w;
    w.show();

    // app.setProperty("RandomColor", false);

    // cstyle_rtmidi();
    // cppstyle_rtmidi();
    // cpppointer_style_rtmidi();
    QRtMidiWrapper midi;
    midi.listen();

    return app.exec();
}
