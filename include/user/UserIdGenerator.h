#ifndef USERIDGENERATOR_H
#define USERIDGENERATOR_H

#include "user/dstypes.h"

class UserIdGenerator {

    public:
        UserIdGenerator(IdUserMap* umap);
        virtual ~UserIdGenerator();
        unsigned int generate();

    private:
        unsigned int current;
        IdUserMap* id_umap;

};

#endif // USERIDGENERATOR_H
