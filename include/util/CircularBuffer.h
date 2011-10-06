#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H


class CircularBuffer {

    public:
        CircularBuffer(int capacity);
        virtual ~CircularBuffer();

        /**
            Adds data to buffer
            return how many bytes sucessfully added
        */
        int add(char* buf, int len);

        /**
            Retrieves data from buffer and frees space
            return how many bytes successfully retrieved
        */
        int get(char* buf, int len);

        /**
            Reads data from buffer
            return how many bytes read
        */
        int read(char* buf, int len);

        /**
            return current size
        */
        int size();

    private:
        char* buf;
        int capacity, start, end, count;
        char read(int index);
        void write(int index, char val);

};

#endif // CIRCULARBUFFER_H
