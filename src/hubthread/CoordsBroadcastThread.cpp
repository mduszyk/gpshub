#include <iostream>
#include <iterator>
#include <ext/hash_set>
#include "util/hashmaphelper.h"
#include "hubthread/CoordsBroadcastThread.h"
#include "socket/netutil.h"
#include "log/macros.h"


CoordsBroadcastThread::CoordsBroadcastThread(IdUserMap* id_umap, NickUserMap* nick_umap, BlockingQueue<int>* uqueue, Socket* udpSocket) {
    this->id_umap = id_umap;
    this->nick_umap = nick_umap;
    this->uqueue = uqueue;
    this->udpSocket = udpSocket;
    // this way of creating buffer causes problems (later segmentation fault), but why?
    //buf = (char*) malloc(16);
}

CoordsBroadcastThread::~CoordsBroadcastThread() {
    //free(buf);
}

void CoordsBroadcastThread::run() {
    User* u;
    Coordinates* coords;

    LOG_INFO("Starting broadcasting thread: " << threadId);

    while(true) {
        int id = uqueue->pull();
        u = id_umap->get(id);
        LOG_DEBUG("Thread: " << threadId << ", pulled user: " << u->getNick());
        if (u == NULL)
            continue;

        coords = u->getReady();
        if (coords != NULL) {
            Coordinates coordsCopy = *coords;

            broadcast(u, &coordsCopy);

            /* Important to set ready to NULL after broadcasting, because
               until ready is NULL GpsDataServer won't add notifications to queue.
               Thanks to that id of particular user can by on queue only once at time. */
            u->setReady(NULL);
        } else {
            LOG_DEBUG("Got null coords");
        }
    }

    LOG_INFO("CoordsBroadcastThread end: " << threadId);
}

void CoordsBroadcastThread::broadcast(User* u, Coordinates* coords) {
    LOG_DEBUG("broadcasting: " << u->getNick());

    BuddiesSet buddies = u->getBuddies();
    ScopeLockRd readlock(buddies.getLock());

    __gnu_cxx::hash_set<char*, StrHash, StrEqual>::iterator end = buddies.getSet().end();

    int pkg_len;
    for (__gnu_cxx::hash_set<char*, StrHash, StrEqual>::iterator nick = buddies.getSet().begin(); nick != end; ++nick) {
        if (nick_umap->count(*nick) > 0) {

            User* buddy = nick_umap->get(*nick);
            if (!buddy->isUdpReady()) {
                // user has uninitialized UDP
                continue;
            }

            tobytes(buf, 0, u->getId());
            tobytes(buf, 4, coords->longitude);
            tobytes(buf, 8, coords->latitude);
            pkg_len = 12;
            if (coords->altitude > 0) {
                tobytes(buf, 12, coords->altitude);
                pkg_len = 16;
            }

            udpSocket->Sendto(buf, pkg_len, buddy->getAddrUdpPtr());
            LOG_DEBUG(u->getNick() << " -> " << *nick);
        }
    }

}
