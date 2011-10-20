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
    buf_pkg = (char*) malloc(16);
}

CoordsBroadcastThread::~CoordsBroadcastThread() {
    free(buf_pkg);
}

void CoordsBroadcastThread::run() {
    User* u;
    Coordinates* coords;

    LOG_INFO("Starting broadcasting thread: " << threadId);

    while (true) {
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

    BuddiesSet buddies = u->getBuddies();
    ScopeLockRd readlock(buddies.getLock());

    __gnu_cxx::hash_set<char*, StrHash, StrEqual>::iterator end = buddies.getSet().end();

    int pkg_len = 12;
    for (__gnu_cxx::hash_set<char*, StrHash, StrEqual>::iterator nick = buddies.getSet().begin(); nick != end; ++nick) {
        if (nick_umap->count(*nick) > 0) {

            User* buddy = nick_umap->get(*nick);
            if (!buddy->isUdpReady()) {
                // user has uninitialized UDP
                continue;
            }

            tobytes(buf_pkg, 0, u->getId());
            tobytes(buf_pkg, 4, coords->longitude);
            tobytes(buf_pkg, 8, coords->latitude);
            if (coords->altitude > 0) {
                tobytes(buf_pkg, 12, coords->altitude);
                pkg_len = 16;
            }

            udpSocket->Sendto(buf_pkg, pkg_len, buddy->getAddrUdpPtr());
            LOG_DEBUG(u->getNick() << " -> " << *nick);
        }
    }

}
