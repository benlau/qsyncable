import QtQuick 2.0

ListView {
    id: board

    orientation: ListView.Horizontal
    model: CardListStore

    add: Transition {
        NumberAnimation {
            properties: "opacity";
            from: 0;
            to: 1;
            duration:200;
            easing.type: Easing.OutQuad;
        }
    }

    delegate: CardList {
        height: parent.height
        title: model.title
        cards: model.cards
    }
}

