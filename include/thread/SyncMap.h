#ifndef SYNCMAP_H
#define SYNCMAP_H

#include <functional>
#include <map>
#include "thread/RwLock.h"
#include "thread/ScopeLockWr.h"
#include "thread/ScopeLockRd.h"


template<class K, class V, class Compare = std::less<K>>
class SyncMap {

    public:
        SyncMap();
        virtual ~SyncMap();
        void put(K key, V value);
        V get(K key);
        int count(K key);
        void erase(K key);

    private:
        std::map<K, V, Compare> map;
        RwLock mlock;

};

// The declarations and definitions of the class template
// member functions should all be in the same header file.

template<class K, class V, class Compare>
SyncMap<K, V, Compare>::SyncMap() {
    //ctor
}

template<class K, class V, class Compare>
SyncMap<K, V, Compare>::~SyncMap() {
    //dtor
}

template<class K, class V, class Compare>
void SyncMap<K, V, Compare>::put(K key, V value) {
    ScopeLockWr writelock(mlock);
    map[key] = value;
}

template<class K, class V, class Compare>
V SyncMap<K, V, Compare>::get(K key) {
    ScopeLockRd readlock(mlock);
    return map[key];
}

template<class K, class V, class Compare>
int SyncMap<K, V, Compare>::count(K key) {
    ScopeLockRd readlock(mlock);
    return map.count(key);
}

template<class K, class V, class Compare>
void SyncMap<K, V, Compare>::erase(K key) {
    ScopeLockWr writelock(mlock);
    map.erase(key);
}

#endif // SYNCMAP_H
