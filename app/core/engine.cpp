#include "engine.h"

#include "serial.h"
#include "crc16.h"
#include "messages_model.h"

#include <QTimer>
#include <QQuickStyle>
#include <QDataStream>
#include <QApplication>
#include <QQmlContext>
#include <QDateTime>
#include <QSerialPort>

#include <tuple>

// которое будет отображать принятые через последовательный интерфейс(/dev/ttyUSB0) сообщения.
// Каждое сообщение имеет вид:
// - адрес отправителя(1 байт)
// - 1...50 - тип (1 байт)
// - 1...7 - размер данных в байтах(2 байта)
// - данные(для сообщения типа 1 - числа в int32, для сообщения типа 2 - текст в utf8, для типа 3..7 - данные отсутсвуют)
// - crc16

namespace {

QString appDir()
{
    return QApplication::applicationDirPath() + "/";
}

std::tuple<bool, QVector<qint32>> type1(int size, QDataStream& ds)
{
    if (size % 4 != 0)
    {
        qDebug() << "непорядок...";
        return {false, QVector<qint32>()};
    }

    QVector<qint32> data;
    for (int i = 0; i < size / 4; ++i)
    {
        qint32 d;
        ds >> d;
        data.push_back(d);
    }
    return {true, data};
}

QString type2(int size, QDataStream& ds)
{
    QByteArray ba;
    for (int i = 0; i < size; ++i)
    {
        quint8 ch;
        ds >> ch;
        ba.append(ch);
    }

    return QString::fromUtf8(ba);
}

bool checkCrc(QByteArray& ba)
{
    const QByteArray lastTwoBytes = ba.right(2); // в последних 2-х байтах расположена CRC
    QDataStream ds(lastTwoBytes);
    ds.setByteOrder(QDataStream::BigEndian);
    quint16 receivedCrc = 0;
    ds >> receivedCrc;

    const QByteArray receivedData = ba.chopped(2);
    const quint16 calculatedCrc = calcCrc(receivedData);

    if (calculatedCrc != receivedCrc)
    {
        qDebug() << "crc not valid";
        return false;
    }

    return true;
}

}

Engine::Engine(QObject* parent)
    : QObject(parent)
    , _qmlEngine(new QQmlApplicationEngine())
    , _chatLogModel(new MessagesModel("chat.log"))
{
    QQuickStyle::setStyle("Fusion");

    serial().setPortName("/dev/ttyUSB0");
    serial().setBaudRate(115200);
    serial().setDataBits(QSerialPort::Data8);
    serial().setParity(QSerialPort::NoParity);
    serial().setStopBits(QSerialPort::OneStop);
    serial().open();

    qDebug() << "lines in log: " << _chatLogModel->rowCount();

    _qmlEngine->rootContext()->setContextProperty("Serial", &serial());
    _qmlEngine->rootContext()->setContextProperty("LogModel", _chatLogModel.data());
    _qmlEngine->load(QUrl::fromLocalFile(appDir() + QString("gui/main.qml")));

    //test();

    connect(&serial(), &Serial::data, this, [this](QByteArray ba)
    {
        bool ok = checkCrc(ba);

        if (!ok)
            return;

        quint8 address;
        quint8 type;
        quint16 size;

        QDataStream ds(ba);
        ds.setByteOrder(QDataStream::BigEndian);

        ds >> address >> type >> size;

        QString text;

        if (type == 1)
        {
            auto [ok, data] = type1(size, ds);

            if (!ok)
                return;

            for (qint32 value : data)
                text += QString(" %1").arg(value); // числа перевел в текст и разделил пробелами
        }

        if (type == 2)
            text = type2(size, ds);

        QString msg = QString("%1\t%2\t%3\t[%4]").arg(address).arg(type).arg(size).arg(text);
        _chatLogModel->append(msg);
    });

    connect(&serial(), &Serial::message, this, [this](QString msg)
    {
        qDebug() << msg;
    });
}

Engine::~Engine()
{

}

void Engine::test()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]()
    {
        static int i = 0;
        _chatLogModel->append(QString::number(i++) + " " + QString::number(QDateTime::currentMSecsSinceEpoch()));
    });

    timer->start(100);
}
