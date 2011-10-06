#ifndef SYNCHASHSET_H
#define SYNCHASHSET_H

#include <ext/hash_set>
#include <iterator>
#include "thread/RwLock.h"
#include "thread/ScopeLockWr.h"
#include "thread/ScopeLockRd.h"

using namespace __gnu_cxx;


template<class Item, class Hash, class Equal>
class SyncHashSet {

    public:
        SyncHashSet();
        virtual ~SyncHashSet();
        void insert(Item item);
        void erase(Item item);
        int count(Item item);
        RwLock& getLock();
        hash_set<Item, Hash, Equal>& getSet();

    private:
        hash_set<Item, Hash, Equal> set;
        RwLock mlock;

};

template<class Item, class Hash, class Equal>
SyncHashSet<Item, Hash, Equal>::SyncHashSet() {

}

template<class Item, class Hash, class Equal>
SyncHashSet<Item, Hash, Equal>::~SyncHashSet() {

}

template<class Item, class Hash, class Equal>
RwLock& SyncHashSet<Item, Hash, Equal>::getLock() {
    return mlock;
}

template<class Item, class Hash, class Equal>
void SyncHashSet<Item, Hash, Equal>::insert(Item item) {
    ScopeLockWr writelock(mlock);
    set.insert(item);
}

template<class Item, class Hash, class Equal>
int SyncHashSet<Item, Hash, Equal>::count(Item item) {
    ScopeLockRd readlock(mlock);
    return set.count(item);
}

template<class Item, class Hash, class Equal>
void SyncHashSet<Item, Hash, Equal>::erase(Item item) {
    ScopeLockWr writelock(mlock);
    set.erase(item);
}

template<class Item, class Hash, class Equal>
hash_set<Item, Hash, Equal>& SyncHashSet<Item, Hash, Equal>::getSet() {
    return set;
}

#endif // SYNCHASHSET_H
