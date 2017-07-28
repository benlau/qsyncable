#ifndef FASTDIFTESTS_H
#define FASTDIFTESTS_H

#include <QObject>

/// Unit tests for fast diff algorithm
class FastDiffTests : public QObject
{
    Q_OBJECT
public:
    explicit FastDiffTests(QObject *parent = nullptr);

signals:

private slots:
    void test_QSImmutable_wrapper();

    void test_QSFastDiffRunnerAlgo();

    void test_QSFastDiffRunner();
    void test_QSFastDiffRunner_data();
};

#endif // FASTDIFTESTS_H
