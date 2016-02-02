#ifndef QSYNCABLETESTS_H
#define QSYNCABLETESTS_H

#include <QObject>

class QSyncableTests : public QObject
{
    Q_OBJECT
public:
    explicit QSyncableTests(QObject *parent = 0);

signals:

public slots:

private slots:
    void patch();
    void patch_merge();

    void tree();
    void tree_updateMin();
    void tree_remove();

    void diffRunner();
    void diffRunner_data();

    void diffRunner_move();
    void diffRunner_move_data();

    void diffRunner_failedCase();
    void diffRunner_failedCase_data();

    void diffRunner_noKeyField();
    void diffRunner_noKeyField_data();

    void diffRunner_invalidKey();

    void diffRunner_random();
    void diffRunner_randomMove();

    void listModel_insert();
    void listModel_roleNames();

};

#endif // QSYNCABLETESTS_H
