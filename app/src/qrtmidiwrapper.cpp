
#include "qrtmidiwrapper.h"
#include <QDebug>

QRtMidiWrapper::QRtMidiWrapper(QObject *parent)
    : QObject{parent} {
}

void QRtMidiWrapper::listen() {
    unsigned int nPorts = m_midiin->getPortCount();
    if (nPorts == 0) {
        std::cout << "No ports available!\n";
        return;
    } else {
        qDebug() << "nPorts: " << nPorts;
        std::string portName;
        for (unsigned int i = 0; i < nPorts; i++) {
            try {
                portName = m_midiin->getPortName(i);
            } catch (RtMidiError &error) {
                error.printMessage();
                return;
            }
            qDebug() << "  Input Port #" << i << ": " << portName;
        }
    }

    try {
        m_midiin->openPort(0);
    } catch (RtMidiError &error) {
        error.printMessage();
        return;
    }

    m_midiin->setCallback([](double deltatime, std::vector<unsigned char> *message, void *userData) {
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++)
            std::cout << "Byte " << i << " = " << (int) message->at(i) << ", ";
        if (nBytes > 0)
            std::cout << "stamp = " << deltatime << std::endl;
    });

    // Don't ignore sysex, timing, or active sensing messages.
    m_midiin->ignoreTypes(false, false, false);
}

void QRtMidiWrapper::cstyle_rtmidi() {
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

void QRtMidiWrapper::cppstyle_rtmidi() {
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

void QRtMidiWrapper::cpppointer_style_rtmidi() {
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
