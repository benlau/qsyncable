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

    void diffRunner();
    void diffRunner_data();

    void diffRunner_noKeyField();
    void diffRunner_noKeyField_data();


    void listModel();

};

#endif // QSYNCABLETESTS_H
