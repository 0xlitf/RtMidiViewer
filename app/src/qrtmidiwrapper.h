#pragma once

#include <QObject>
#include <cstdlib>
#include <vector>
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

signals:

private:
    RtMidiIn *m_midiin = new RtMidiIn();
};
