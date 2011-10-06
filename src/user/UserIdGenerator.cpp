#include "user/UserIdGenerator.h"
#include "limits.h"

UserIdGenerator::UserIdGenerator(IdUserMap* id_umap) {
    this->current = 0;
    this->id_umap = id_umap;
}

UserIdGenerator::~UserIdGenerator() {
    //dtor
}

unsigned int UserIdGenerator::generate() {
    unsigned int n = 0;
    while (n <= ULONG_MAX) {
        current++;
        if (this->id_umap->count(current) == 0 && current > 0) {
            return current;
        }
        n++;
    }

    // 0 is forbidden id - it means that all ids are taken
    return 0;
}
