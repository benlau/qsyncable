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
    void change();

    void diffRunner();
    void diffRunner_data();

};

#endif // QSYNCABLETESTS_H
