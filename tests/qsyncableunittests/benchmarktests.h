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
    void noChange();
    void noChange_data();

    void moveOne();
    void moveOne_data();
};

#endif // BENCHMARKTESTS_H
