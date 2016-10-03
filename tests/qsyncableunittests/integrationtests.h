#ifndef INTEGRATIONTESTS_H
#define INTEGRATIONTESTS_H

#include <QObject>

/// Test integration with other component
class IntegrationTests : public QObject
{
    Q_OBJECT
public:
    explicit IntegrationTests(QObject *parent = 0);

signals:

public slots:

private slots:

    void sortFilterProxyModel();

    void test_assign();

    void test_get();

    void test_set();

};

#endif // INTEGRATIONTESTS_H
