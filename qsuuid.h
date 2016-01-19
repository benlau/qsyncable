#ifndef QSUUID_H
#define QSUUID_H

#include <QObject>

class QSUuid : public QObject
{
    Q_OBJECT
public:
    explicit QSUuid(QObject *parent = 0);

public slots:

    QString create() const;

};

#endif // QSUUID_H
