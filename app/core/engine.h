#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QQmlApplicationEngine>

class Serial;
class MessagesModel;

class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(QObject* parent = nullptr);
    ~Engine();

private:
    void test();

    QScopedPointer<QQmlApplicationEngine> _qmlEngine;
    QScopedPointer<MessagesModel> _chatLogModel;
};
