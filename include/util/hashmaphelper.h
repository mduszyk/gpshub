#ifndef HASHMAPHELPER_H_INCLUDED
#define HASHMAPHELPER_H_INCLUDED

#include <locale>
#include <cstring>
#include <functional>

struct StrCompare : public std::binary_function<const char*, const char*, bool> {

    public:
        bool operator() (const char* str1, const char* str2) const {
            return std::strcmp(str1, str2) < 0;
        }

};

struct StrHash {

    std::locale loc; // the "C" locale
    const std::collate<char>& coll;

    StrHash() : coll(std::use_facet<std::collate<char> >(loc)) {
    }

    size_t operator()(const char* str) const {
        return coll.hash(str, str + strlen(str));
    }

};

struct StrEqual {
    bool operator()(const char* str1, const char* str2) const {
        return std::strcmp(str1, str2) == 0;
    }
};

struct IntHash {
    size_t operator()(const int i) const {
        // no hashing needed ;)
        return i;
    }
};

struct IntEqual {
    bool operator()(const int i1, const int i2) const {
        return i1 == i2;
    }
};


#endif // HASHMAPHELPER_H_INCLUDED
