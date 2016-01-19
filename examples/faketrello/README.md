Fake Trello
==========

![screenshot](https://raw.githubusercontent.com/benlau/qsyncable/master/examples/faketrello/docs/screenshot.png)


Demonstration

1. How to hold application data by implicitly sharing class in C++ and update UI by sync function
2. How to setup nested list model by JsonModel

Sync function
-------------

appdelegate.cpp
```
void AppDelegate::sync()
{
    QVariantMap map = m_board.toMap();

    QVariantList lists = map["lists"].toList();

    // It is important to set an unique key field on input data.
    // Otherwise, it won't be able to generate insertion, removal and move patch.
    QSDiffRunner runner;
    runner.setKeyField("uuid");

    QList<QSPatch> patches = runner.compare(cardListStore->storage(),
                                            lists);

    runner.patch(cardListStore, patches);
}
```

Nested List Model
-----------------

views/CardList.qml

```

model: JsonModel {
    keyField: "uuid"
    source: cardList.cards
}

```
