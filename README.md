QSyncable - Synchronize data between models
===========================================

QSyncable provides an QML friendly list model (based on QAbstractItemModel) as a wrapper of any data structure from C++ / Javascript. Instead to access data from original source per query, it duplicates a copy of data locally, and keep updated by an average O(n) synchronization algorithm. Every update is carried by passing a full copy of data snapshot. It will find out the diff and transform into a list of change operations like insertion, removal and move. It will guarantee the behaviour is identical to the original QML ListModel. Therefore, UI components could react to the changes correctly.

An immediate benefit of using QSycnable is the simplification of data pipeline. If you need your UI to respond for changes like insertion / removal correctly, you must update the ListModel by the corresponding method explicitly. QSyncable combines all kinds of update methods into a single way. Such that user doesn’t need to care about their differences and setup data binding by just a single connection.

Moreover, QSyncable could also be used as a solution for the nested list model.

**Remarks: This project is still under development. Do not use it.**

How it works?
-------------

![Workflow](https://raw.githubusercontent.com/benlau/qsyncable/master/docs/qsyncable-workflow.png)

**DiffRunner (QSDiffRunner)** compares two QVariantList to produce a patch for transforming one of the list to another list with minimum no. of steps. The result can be applied on a QSListModel. DiffRunner use an average O(n) algorithm and therefore it should be fast enough for regular UI application.

**ListModel (QSListModel)** is an implementation of QAbstactItemModel. It stores data in a list of QVariantMap. It will emit insert, remove, move and data changed signals according to the patch applied.



QSyncable provides the two classes above for user to convert their own data structure to a QML friendly list model. Usually there are several ways to update a list model. QSyncable combines all of update methods into a single process - patching.

Whatever the data source has been changed, regardless of update method, user converts it into a QVariantList and pass it to DiffRunner to compare with current snapshot. Then apply the generated patch on QSListModel. UI component will be refreshed according to the emitted signals from QSListModel.


The diagram below shows an example application architecture using QSyncable:

![QSyncable Application Architecture](https://raw.githubusercontent.com/benlau/qsyncable/master/docs/qsyncable-application-architecture-example.png)

In QSyncable application, ListModel only keep a copy of the data. it is meaningless for UI components to modify it. Instead, UI components should ask to update the data source and trigger synchronization afterward. The component for “updates” and “queries” is in fact separated. (More information in this [article](https://medium.com/@benlaud/action-dispatcher-design-pattern-for-qml-c350b1d2a7e7#.mi3b8hbuv) )

Design Principle - Separation of "updates" and "queries"
----------

QSyncable is designed to solve a fundamental problem of C++ & QML application: How to share data between C++ and QML?

QObject list model is definitely a bad idea. It is terrible to manage their life cycle and ownership (QML / C++ scope). You should beaware of garbage collection in your QObject list model even it is written by C++.

Using a variant list model is better, but it is not C++ friendly. And it is difficult to handle nested list model.

In fact, the problem will be simple if you separate “updates” and “queries” into different components.
First of all, you don’t even need to consider QObject list model approach.
It has no any advantage of using QObject list model if you use other component for update.

Moreover, it is not necessary to use a variant list model as a central data source. You may use any data structure you like. Leave variant list model for presentation only.

QSyncable takes a step further. It simplifies the process to update the variant list model from a data source by combining insertion, removal, move and change operations into a single process - patching, while maintaining the correctness of UI response. It solves not only the problem of C++ and QML data sharing, but also a solution of nested list model within QML.

Reference:

1. [What the Flux? (On Flux, DDD, and CQRS) — Jack Hsu](http://jaysoo.ca/2015/02/06/what-the-flux/)

Why use QSyncable for C++?
--------------------------

(1) The function of QQmlListProperty is limited.

(2) Implement your own QAbstactItemModel is troublesome.

You need to understand how QAbstactItemModel works and emit insert, remove, move and update signals correctly. Otherwise, UI components like ListView will not react correctly.

(3) Use implicit sharing class over QObject

QObject is not copyable and you need to manage its life cycle. It is not really a good solution as a data storage class.

Implicit sharing class is recommended for this purpose. Because it can safely be copied across threads, like any other QVariant classes.
Once you find that your data is too big for processing, you may pass it to a thread and work in the background.

[Implicit Sharing | Qt Core 5.5](http://doc.qt.io/qt-5/implicit-sharing.html)

(4) Works for any data structure

You just need to write a conversion function, QSyncable will do the rest for you.

(5) Simple update method

No matter what kind of update happen, just convert your data structure to QVariantList, pass it to DiffRunner, then patch a model.

Why use QSyncable for QML?
--------------------------

(1) Use JsonModel to wrap your Javascript object.

(2) Able to work as a nested list model.

(3) Simple data pipeline.

Algorithm
---------

The average time complexity of the diff algorithm in QSDiffRunner is O(n), where n is the sum of the number of items in comparison.
But it is not the worst case time complexity. It is O(n + m log m), where m is the no. of block move operations.

However, unless you are doing a reverse or random shuffle.
Moving a single block of items, regardless of the number, from one to another place will remain in O(n) time complexity.

If you really need to do a reverse of a list or random shuffle with large amount of data, you may consider to move it to a thread or turn off the key field checking.
Just don’t set the key field on QSDiffRunner, then it will ignore the insertion, removal and moving checking.

| Condition                    | Time Complexity             |
|------------------------------|-----------------------------|
| Insert to an empty list      | O(1)                        |
| Clear the list               | O(1)                        |
| Insert items to any position | O(n)                        |
| Remove a block of items      | O(n)                        |
| Move a block of items        | O(n)                        |
| Reverse the list             | O(n + m log m)              |
| Random shuffle               | O(n + m log m)              |

Installation
------------


Class Reference
---------------

[QSyncable - Class Reference](http://benlau.github.io/qsyncable/)

Example
-------

[faketrello](https://github.com/benlau/qsyncable/tree/master/examples/faketrello) - Simulate a Trello card board.


