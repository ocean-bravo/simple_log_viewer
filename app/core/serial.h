#pragma once

#include <QObject>
#include <QScopedPointer>

#include "singleton.h"

class QThread;
class SerialPrivate;
class QSerialPort;

class Serial : public QObject, public Singleton<Serial>
{
    Q_OBJECT

public:
    bool isOpen();

public slots:
    void open();
    void close();
    void write(const QByteArray& msg);
    void setBaudRate(qint32 br);
    void setDataBits(int db);
    void setParity(int p);
    void setStopBits(int sb);
    void setPortName(const QString& pn);

signals:
    void data(const QByteArray& ba);
    void message(const QString& msg);
    void connected();
    void disconnected();

protected:
    explicit Serial(QObject* parent = nullptr);
    ~Serial();
    SerialPrivate* const _impl;
    QScopedPointer<QThread> _thread;

    friend class Singleton<Serial>;
};


class SerialPrivate : public QObject
{
    Q_OBJECT

public:
    explicit SerialPrivate(QObject* parent = nullptr);
    virtual ~SerialPrivate();

    bool isOpen() const;

public slots:
    void open();
    void close();
    void write(const QByteArray& msg);
    void setBaudRate(qint32 br);
    void setDataBits(int db);
    void setParity(int p);
    void setStopBits(int sb);
    void setPortName(const QString& pn);

signals:
    void data(const QByteArray& ba);
    void message(const QString& msg);
    void connected();
    void disconnected();

private:
    void read();

    QSerialPort* _port = nullptr;
    QByteArray _buffer;
};

inline Serial& serial()
{
    return Serial::instance();
}

