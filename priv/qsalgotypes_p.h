#pragma once

namespace QSAlgoTypes {

    enum Type {
        NoMove,
        Insert,
        Remove,
        Move
    };

    class State {
    public:
        inline State(int from = -1 , int to = -1) {
            this->posF = from;
            this->posT = to;
            isMoved = false;
        }

        // The position in "from" list
        int posF;

        // The position in "to" list
        int posT;

        // Is it moved?
        bool isMoved;
    };


    // Move data storage
    class MoveOp {
    public:
        inline MoveOp(int indexF = -1, int from = -1, int to = -1, int count = 1) : posF(indexF) , from(from), to(to), count(count) {

        }

        inline bool isNull() const {
            return posF < 0;
        }

        inline bool canMerge(const MoveOp& other) const {
            return posF + count == other.posF && to + count == other.to ;
        }

        inline void merge(const MoveOp& other) {
            count += other.count;
        }

        inline void clear() {
            posF = from = to = -1;
        }

        // The absolute position in "from"
        int posF;

        int from;

        int to;

        int count;
    };

}
