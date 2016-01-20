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

    void noChange();
    void noChange_data();

    void changeOne();
    void changeOne_data();

    void moveOne();
    void moveOne_data();

    void insertAll();
    void insertAll_data();

    void changeAll();
    void changeAll_data();

    void removeAll();
    void removeAll_data();

private:
    void setup();
    QList<int> sizes;
};

#endif // BENCHMARKTESTS_H
