#include "messages_model.h"

#include <QDebug>
#include <QTextStream>

MessagesModel::MessagesModel(const QString& filename, QObject* parent)
    : QStringListModel(parent)
    , _logFile(new QFile(filename))
{
    if (!_logFile->open(QIODevice::ReadOnly))
        qDebug() << QString("could not open file %1 for reading").arg(filename);

    const QByteArray ba = _logFile->readAll();

    const QList<QByteArray> lines = ba.split('\n');

    for (const QByteArray& line : lines)
        appendToModel(line);

    _logFile->close();

    if (!_logFile->open(QIODevice::WriteOnly | QIODevice::Append))
        qDebug() << QString("could not open file %1 for writing").arg(filename);

    connect(this, &MessagesModel::rowsInserted, this, [this]()
    {
        emit newRows();
    });
}

void MessagesModel::append(const QString& msg)
{
    appendToFile(msg);
    appendToModel(msg);
}

void MessagesModel::appendToFile(const QString& msg)
{
    QTextStream out(_logFile.data());
    out << msg;
    Qt::endl(out);
}

void MessagesModel::appendToModel(const QString& msg)
{
    const int row = rowCount();
    insertRows(row, 1);
    QModelIndex idx = index(row);
    setData(idx, msg);
}
