#include "serial.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QThread>

namespace {

// Последовательность 0x7D 0x5D заменяется на 0x7D
// Последовательность 0x7D 0x5E заменяется на 0x7E
QByteArray removeEscape(const QByteArray& ba)
{
    QByteArray buf;

    for (int i = 1; i < ba.size() - 1; ++i)
    {
        char element = ba[i];
        char nextElement = ba[i + 1];

        if (element == char(0x7D) && nextElement == char(0x5D))
        {
            buf.append(char(0x7D));
            ++i;
        }
        else if (element == char(0x7D) && nextElement == char(0x5E))
        {
            buf.append(char(0x7E));
            ++i;
        }
        else
        {
            buf.append(element);
        }
    }
    return buf;
}

}


SerialPrivate::SerialPrivate(QObject* parent)
    : QObject(parent)
    , _port(new QSerialPort(this))
{
    connect(_port, &QSerialPort::readyRead, this, &SerialPrivate::read, Qt::QueuedConnection);
}

SerialPrivate::~SerialPrivate()
{}

// Сообщение находится между байтами 0x7E и 0x7D
void SerialPrivate::read()
{
    const QByteArray ba = _port->readAll();

    _buffer.append(ba);

    const int beginByteIndex = _buffer.indexOf(char(0x7E));

    if (beginByteIndex == -1)
    {
        _buffer.clear(); // В буфере нет начала, выкидываем
        return;
    }

    const int endByteIndex = _buffer.indexOf(char(0x7D), beginByteIndex + 1);

    if (endByteIndex == -1)
        return; // В буфере нет конца, ждем дальше

    QByteArray packet = _buffer.mid(beginByteIndex + 1, endByteIndex - beginByteIndex - 1);

    _buffer.remove(0, endByteIndex + 1);

    if (packet.size() < 6)
        return; // слишком короткий пакет

    packet = removeEscape(packet);

    emit data(packet);
}

void SerialPrivate::setBaudRate(qint32 br)
{
    _port->setBaudRate(br);
}

void SerialPrivate::setDataBits(int db)
{
    _port->setDataBits(QSerialPort::DataBits(db));
}

void SerialPrivate::setParity(int p)
{
    _port->setParity(QSerialPort::Parity(p));
}

void SerialPrivate::setStopBits(int sb)
{
    _port->setStopBits(QSerialPort::StopBits(sb));
}

void SerialPrivate::setPortName(const QString& pn)
{
    _port->setPortName(pn);
}

void SerialPrivate::open()
{
    if (!_port->open(QSerialPort::ReadWrite))
    {
        emit message(_port->portName() + " port is not opened");
    }
    else
    {
        emit message(_port->portName() + " port is opened");
        emit connected();
    }
}

void SerialPrivate::close()
{
    if (!_port->isOpen())
        return;
    _port->close();

    emit message(_port->portName() + " port is closed");
    emit disconnected();
}

bool SerialPrivate::isOpen() const
{
    return _port->isOpen();
}

void SerialPrivate::write(const QByteArray& msg)
{
    _port->write(msg);
}

Serial::Serial(QObject* parent)
    : QObject(parent)
    , _impl(new SerialPrivate)
    , _thread(new QThread)
{
    connect(_impl, &SerialPrivate::data,         this, &Serial::data, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::message,      this, &Serial::message, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::connected,    this, &Serial::connected, Qt::QueuedConnection);
    connect(_impl, &SerialPrivate::disconnected, this, &Serial::disconnected, Qt::QueuedConnection);

    connect(_thread.data(), &QThread::finished, _impl, &QObject::deleteLater);

    _impl->moveToThread(_thread.data());
    _thread->start();
}

Serial::~Serial()
{
    close();
    _thread->quit();
    _thread->wait(1000);
}

void Serial::setBaudRate(qint32 br)
{
    QMetaObject::invokeMethod(_impl, "setBaudRate", Qt::QueuedConnection, Q_ARG(qint32, br));
}

void Serial::setDataBits(int db)
{
    QMetaObject::invokeMethod(_impl, "setDataBits", Qt::QueuedConnection, Q_ARG(int, db));
}

void Serial::setParity(int p)
{
    QMetaObject::invokeMethod(_impl, "setParity", Qt::QueuedConnection, Q_ARG(int, p));
}

void Serial::setStopBits(int sb)
{
    QMetaObject::invokeMethod(_impl, "setStopBits", Qt::QueuedConnection, Q_ARG(int, sb));
}

void Serial::setPortName(const QString& pn)
{
    QMetaObject::invokeMethod(_impl, "setPortName", Qt::QueuedConnection,  Q_ARG(QString, pn));
}

void Serial::write(const QByteArray& msg)
{
    QMetaObject::invokeMethod(_impl, "write", Qt::QueuedConnection, Q_ARG(QByteArray, msg));
}

void Serial::open()
{
    QMetaObject::invokeMethod(_impl, "open", Qt::QueuedConnection);
}

void Serial::close()
{
    QMetaObject::invokeMethod(_impl, "close", Qt::QueuedConnection);
}
