#ifndef BENCHMARKTESTS_H
#define BENCHMARKTESTS_H

#include <QObject>

class BenchmarkTests : public QObject
{
    Q_OBJECT
public:
    explicit BenchmarkTests(QObject *parent = 0);

signals:

public slots:

private slots:
    void copy();

    void hashing();

    // O(n)

    void noChange();
    void noChange_data();

    void changeOne();
    void changeOne_data();

    void changeAll();
    void changeAll_data();

    void moveOne();
    void moveOne_data();

    void removeOne();
    void removeOne_data();

    // O(n + m log m)

    void moveAll();
    void moveAll_data();

    void reverse();
    void reverse_data();

    // O(1)
    void insertAll();
    void insertAll_data();

    void removeAll();
    void removeAll_data();

private:
    void setup();
    QList<int> sizes;
};

#endif // BENCHMARKTESTS_H
