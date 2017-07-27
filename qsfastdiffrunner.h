#pragma once
#include <priv/qsdiffrunneralgo_p.h>
#include <qspatchable.h>

template <typename T>
class QSFastDiffRunner {
public:
    QSFastDiffRunner() {
    }

    QSPatchSet compare(const QList<T>& from, const QList<T>& to) {
        QSFastDiffRunnerAlgo<T> algo;
        return algo.compare(from , to);
    }

    bool patch(QSPatchable *patchable, const QSPatchSet& patches) const
    {
        QVariantMap diff;
        foreach (QSPatch patch, patches) {
            switch (patch.type()) {
            case QSPatch::Remove:
                patchable->remove(patch.from(), patch.count());
                break;
            case QSPatch::Insert:
                patchable->insert(patch.from(), patch.data());
                break;
            case QSPatch::Move:
                patchable->move(patch.from(), patch.to(), patch.count());
                break;
            case QSPatch::Update:
                if (patch.data().size() > 0) {
                    diff = patch.data().at(0).toMap();
                }
                patchable->set(patch.from(), diff);
                break;
            default:
                break;
            }
        }
        return true;
    }

};
