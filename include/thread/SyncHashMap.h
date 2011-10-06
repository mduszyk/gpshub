#ifndef SYNCHASHMAP_H
#define SYNCHASHMAP_H

#include <ext/hash_map>
#include "thread/RwLock.h"
#include "thread/ScopeLockWr.h"
#include "thread/ScopeLockRd.h"

template<class Key, class Data, class Hash, class Equal>
class SyncHashMap {

    public:
        SyncHashMap();
        virtual ~SyncHashMap();
        void put(Key key, Data data);
        Data get(Key key);
        int count(Key key);
        void erase(Key key);

    private:
        __gnu_cxx::hash_map<Key, Data, Hash, Equal> map;
        RwLock mlock;
};

// The declarations and definitions of the class template
// member functions should all be in the same header file.

template<class Key, class Data, class Hash, class Equal>
SyncHashMap<Key, Data, Hash, Equal>::SyncHashMap() {
    //ctor
}

template<class Key, class Data, class Hash, class Equal>
SyncHashMap<Key, Data, Hash, Equal>::~SyncHashMap() {
    //dtor
}

template<class Key, class Data, class Hash, class Equal>
void SyncHashMap<Key, Data, Hash, Equal>::put(Key key, Data data) {
    ScopeLockWr writelock(mlock);
    map[key] = data;
}

template<class Key, class Data, class Hash, class Equal>
Data SyncHashMap<Key, Data, Hash, Equal>::get(Key key) {
    ScopeLockRd readlock(mlock);
    return map[key];
}

template<class Key, class Data, class Hash, class Equal>
int SyncHashMap<Key, Data, Hash, Equal>::count(Key key) {
    ScopeLockRd readlock(mlock);
    return map.count(key);
}

template<class Key, class Data, class Hash, class Equal>
void SyncHashMap<Key, Data, Hash, Equal>::erase(Key key) {
    ScopeLockWr writelock(mlock);
    map.erase(key);
}


#endif // SYNCHASHMAP_H
