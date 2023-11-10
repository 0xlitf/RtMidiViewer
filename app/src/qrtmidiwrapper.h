#pragma once

#include <QObject>
#include <cstdlib>
#include <vector>
#include <QDebug>
#include <functional>
#include "RtMidi.h"
#include "rtmidi_c.h"

class QRtMidiWrapper : public QObject
{
    Q_OBJECT
public:
    explicit QRtMidiWrapper(QObject *parent = nullptr);
    ~QRtMidiWrapper() {
        delete m_midiin;
    }
    void listen();

    void cstyle_rtmidi();

    void cppstyle_rtmidi();

    void cpppointer_style_rtmidi();

    void printMessage(std::vector<unsigned char> *message) {
        QString uintStr;
        QString hexStr;
        for (auto &row : *message) {
            uintStr += QString::number((int) row) + "\t";
            hexStr += QString::number((int) row, 16) + "\t";
        }
        qDebug() << uintStr;
        qDebug() << hexStr;
    }

    static QRtMidiWrapper &getInstance() {
        static QRtMidiWrapper s;
        return s;
    }

    static void callback(double deltatime, std::vector<unsigned char> *message, void *userData) {
        QList<int> list;

        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++)
            list.append((int)message->at(i));

        emit QRtMidiWrapper::getInstance().dataReceived(deltatime, list, userData);
    }

signals:
    void dataReceived(double deltatime, QList<int> message, void *userData);

private:
    RtMidiIn *m_midiin = new RtMidiIn();

    RtMidiIn::RtMidiCallback m_callback;
};
