#pragma once

#include <QStringListModel>
#include <QScopedPointer>
#include <QFile>

class MessagesModel : public QStringListModel
{
    Q_OBJECT

public:
    MessagesModel(const QString& filename, QObject* parent = nullptr);

    void append(const QString& msg);

signals:
    void newRows();

private:
    void appendToFile(const QString& msg);
    void appendToModel(const QString& msg);

    QScopedPointer<QFile> _logFile;
};
