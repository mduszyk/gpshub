#include "util/CircularBuffer.h"
#include "stdlib.h"

CircularBuffer::CircularBuffer(int capacity) {
    buf = (char*) malloc(capacity);
    start = 0;
    end = 0;
    count = 0;
    this->capacity = capacity;
}

CircularBuffer::~CircularBuffer() {
    free(buf);
}

int CircularBuffer::add(char* buf, int len) {
    int n = 0, j;
    for (int i = 0; i < len; i++) {
        j = (end + i) % capacity;
        if (j == start && i > 0)
            break;
        this->buf[j] = buf[i];
        n++;
    }

    end += n;
    count += n;

    return n;
}

int CircularBuffer::get(char* buf, int len) {
    int n = 0, j;
    for (int i = 0; i < len; i++) {
        j = (start + i) % capacity;
        if (j == end && i > 0)
            break;
        buf[i] = this->buf[j];
        n++;
    }

    start += n;
    count -= n;

    return n;
}

int CircularBuffer::read(char* buf, int len) {
    int n = 0, j;
    for (int i = 0; i < len; i++) {
        j = (start + i) % capacity;
        if (j == end && i > 0)
            break;
        buf[i] = this->buf[j];
        n++;
    }

    return n;
}

int CircularBuffer::size() {
    return count;
}
