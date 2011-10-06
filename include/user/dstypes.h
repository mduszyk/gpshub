// data structures user types

#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

#include "thread/SyncHashMap.h"
#include "thread/SyncHashSet.h"
#include "thread/SyncMap.h"
#include "thread/SyncSet.h"
#include "user/User.h"
#include "util/hashmaphelper.h"

//typedef SyncMap<char*, User*, StrCompare> NickUserMap;
//typedef SyncMap<int, User*> IdUserMap;

//typedef SyncSet<char*, StrCompare> BuddiesSet;

typedef SyncHashMap<char*, User*, StrHash, StrEqual> NickUserMap;
typedef SyncHashMap<int, User*, IntHash, IntEqual> IdUserMap;


#endif // MAPS_H_INCLUDED
