#ifndef SYNCSET_H
#define SYNCSET_H

#include <set>
#include "thread/RwLock.h"
#include "thread/ScopeLockWr.h"
#include "thread/ScopeLockRd.h"


template<class K, class Compare = std::less<K>>
class SyncSet {

    public:
        SyncSet();
        virtual ~SyncSet();
        void insert(K key);
        void erase(K key);
        int count(K key);

    private:
        std::set<K, Compare> set;
        RwLock mlock;

};

template<class K, class Compare>
SyncSet<K, Compare>::SyncSet() {

}

template<class K, class Compare>
SyncSet<K, Compare>::~SyncSet() {

}

template<class K, class Compare>
void SyncSet<K, Compare>::insert(K key) {
    ScopeLockWr writelock(mlock);
    return set.insert(key);
}

template<class K, class Compare>
int SyncSet<K, Compare>::count(K key) {
    ScopeLockRd readlock(mlock);
    return set.count(key);
}

template<class K, class Compare>
void SyncSet<K, Compare>::erase(K key) {
    ScopeLockWr writelock(mlock);
    set.erase(key);
}

#endif // SYNCSET_H
