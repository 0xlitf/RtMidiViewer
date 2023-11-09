
#include <QDebug>
#include "qrtmidiwrapper.h"

QRtMidiWrapper::QRtMidiWrapper(QObject *parent)
    : QObject{parent} {

}

void QRtMidiWrapper::listen()
{
    std::vector<unsigned char> message;
    int nBytes, i;
    double stamp;

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

    // Set our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue.
    // midiin->setCallback(&mycallback);
    m_midiin->setCallback([](double deltatime, std::vector<unsigned char> *message, void *userData) {
        qDebug() << "callback";
        unsigned int nBytes = message->size();
        for (unsigned int i = 0; i < nBytes; i++)
            std::cout << "Byte " << i << " = " << (int) message->at(i) << ", ";
        if (nBytes > 0)
            std::cout << "stamp = " << deltatime << std::endl;
    });

    // Don't ignore sysex, timing, or active sensing messages.
    // midiin->ignoreTypes( false, false, false );

    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);

}
