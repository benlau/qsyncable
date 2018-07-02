#include <QtCore>
#include "board.h"

class BoardPriv : public QSharedData
{
public:
    BoardPriv() {
        nextCardId = 1;
        nextListId = 1;
    }

    QList<List> lists;
    int nextCardId;
    int nextListId;
};

Board::Board() : d(new BoardPriv)
{

}

Board::Board(const Board &rhs) : d(rhs.d)
{

}

Board &Board::operator=(const Board &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

Board::~Board()
{

}

QList<List> Board::lists() const
{
    return d->lists;
}

void Board::setLists(const QList<List> &list)
{
    d->lists = list;
}

int Board::nextCardId() const
{
    return d->nextCardId;
}

void Board::setNextCardId(int nextId)
{
    d->nextCardId = nextId;
}

void Board::addList()
{
    d->lists.append(list());
}

void Board::addCard(const QString &listUuid)
{
    for (int i = 0 ; i < d->lists.size() ; i++) {
        List list = d->lists.at(i);
        if (list.uuid() == listUuid) {
            list.insertCard(0, card());
            d->lists[i] = list;
            break;
        }
    }
}

void Board::moveCard(const QString &listUuid, const QString &fromCardUUid, const QString &toCardUuid)
{
    int index = indexOfList(listUuid);
    if (index < 0) {
        return;
    }

    List list = d->lists.at(index);
    list.moveCard(fromCardUUid, toCardUuid);
    d->lists[index] = list;
}

void Board::moveCard(const QString &fromlistUuid, const QString &fromCardUUid, const QString &toListUuid, const QString &toCardUuid)
{
    if (fromlistUuid == toListUuid) {
        moveCard(fromlistUuid, fromCardUUid, toCardUuid);
        return;
    }

    int fIndex = indexOfList(fromlistUuid);
    int tIndex = indexOfList(toListUuid);

    if (fIndex < 0 || tIndex < 0) {
        return;
    }

    List fromList = d->lists[fIndex];
    List toList = d->lists[tIndex];

    Card card = fromList.cards()[fromList.indexOfCard(fromCardUUid)];
    fromList.removeCard(fromCardUUid);

    int index = toList.indexOfCard(toCardUuid);

    toList.insertCard(index, card);
    d->lists[fIndex] = fromList;
    d->lists[tIndex] = toList;
}

int Board::indexOfList(const QString &listUuid)
{

    for (int i = 0 ; i < d->lists.size() ; i++) {
        if (d->lists.at(i).uuid() == listUuid) {
            return i;
        }
    }

    return -1;
}

void Board::removeCard(const QString& listUuid, const QString& cardUuid)
{
    for (int i = 0 ; i < d->lists.size() ; i++) {
        List list = d->lists.at(i);
        if (list.uuid() == listUuid) {
            list.removeCard(cardUuid);
            d->lists[i] = list;
            break;
        }
    }
}

// NPM: original version of load(), now invoked from load() if persistence file missing or malformed
void Board::failsafeLoad() {
    List list1 = list();

    list1 << card()
          << card()
          << card();

    List list2 = list();
    list2 << card() << card() << card() << card() << card();

    d->lists << list1 << list2;
}

// NPM: load persisted JSON board, or call failsafeload() above if first run or malformed persistence file.
// see https://github.com/benlau/qsyncable/issues/2
void Board::load(const QString& persistFilePath)
{
    QFile file(persistFilePath);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "NOTE: using default board because no persistence file found: "<< persistFilePath;
        Board::failsafeLoad();
    }
    else {
        QTextStream stream (&file);
        QString text = stream.readAll();
        file.close();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &error);

        if (error.error != QJsonParseError::NoError) {
            qWarning() << "JSON::parse() error: "<< error.errorString();
            qWarning() << "NOTE: using default board because persistence file is malformed: "<< persistFilePath;
            Board::failsafeLoad();
        }
        else {
            QVariantMap     map        = doc.object().toVariantMap();
            QList<QVariant> lists      = map.value("lists").toList();
            int             listsCount = lists.size();
            for (int listsIdx = 0 ; listsIdx < listsCount ; listsIdx++) {
                QMap<QString,QVariant> listsMap  = lists[listsIdx].toMap();
                QString                listName  = listsMap.value("title").toString();
                QList<QVariant>        listCards = listsMap.value("cards").toList();
                int                    listNum   = listName.mid(5).toInt(); //"List %1" --> "%1"
                if (d->nextListId <= listNum)
                    d->nextListId = listNum + 1;
                List cardsList(listName);
                int cardsCount = listCards.size();
                for (int cardIdx = 0 ; cardIdx < cardsCount ; cardIdx++) {
                    QString cardName = listCards[cardIdx].toMap().value("text").toString();
                    int     cardNum  = cardName.mid(5).toInt(); //"Card %1" --> "%1"
                    if (d->nextCardId <= cardNum)
                        d->nextCardId = cardNum + 1;
                    Card card(cardName);
                    cardsList << card;
                }
                d->lists << cardsList;
            }
        }
    }
}

QVariantMap Board::toMap() const
{
    QVariantMap res;

    QVariantList lists;
    for (int i = 0 ; i < d->lists.size() ; i++) {
        lists << d->lists[i].toMap();
    }

    res["lists"] = lists;

    return res;
}

Card Board::card()
{
    return Card(QString("Card %1").arg(d->nextCardId++));
}

List Board::list()
{
    return List(QString("List %1").arg(d->nextListId++));
}

