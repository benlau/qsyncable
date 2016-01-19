#ifndef APPDELEGATE_H
#define APPDELEGATE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "qslistmodel.h"
#include "board.h"

class AppDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AppDelegate(QObject *parent = 0);

    int run();

signals:

public slots:

    void addList();

    void removeCard(const QString& listUuid, const QString& cardUUid);

private slots:

    void sync();

private:
    QQmlApplicationEngine m_engine;
    QSListModel* cardListStore;

    // A fake trello board
    Board m_board;
};

#endif // APPDELEGATE_H
