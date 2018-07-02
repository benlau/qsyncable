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

    QByteArray stringifyModel(); //NPM
    void persistModel(const QString& filePath); //NPM
    QByteArray stringifyBoard(); //NPM
    void persistBoard(const QString& filePath); //NPM

    void addList();

    void addCard(const QString& listUuid);

    void removeCard(const QString& listUuid, const QString& cardUUid);

    void moveCard(const QString& listUuid,const QString& fromCardUUid, const QString& toCardUuid);

    void moveCard(const QString& fromListUuid,const QString& fromCardUUid, const QString& toListUuid, const QString& toCardUuid);


private slots:

    void sync();

private:
    QQmlApplicationEngine m_engine;
    QSListModel* cardListStore;

    // A fake trello board
    Board m_board;
};

#endif // APPDELEGATE_H
