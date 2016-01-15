#ifndef QSCHANGE_H
#define QSCHANGE_H

#include <QtCore>
#include <QSharedDataPointer>
#include <QVariantMap>

class QSChangeData;

class QSChange
{
public:
    enum Type {
        Null,
        Insert,
        Remove,
        Update,
        Move
    };

    QSChange();
    QSChange(const QSChange &);
    QSChange &operator=(const QSChange &);
    ~QSChange();

    QSChange::Type type() const;
    void setType(const QSChange::Type &type);

    QVariantMap data() const;

    void setData(const QVariantMap &data);

    bool operator==(const QSChange& rhs) const;

    int from() const;
    void setFrom(int from);

    int to() const;
    void setTo(int to);

    bool isNull() const;

signals:

public slots:

private:
    QSharedDataPointer<QSChangeData> d;
};

QDebug operator<<(QDebug dbg, const QSChange& change);

Q_DECLARE_METATYPE(QSChange)

#endif // QSCHANGE_H
