#ifndef CMDPKG_H
#define CMDPKG_H


class CmdPkg {

    public:

        static const char REGISTER_NICK = 1;
        static const char ADD_BUDDIES = 2;
        static const char REMOVE_BUDDIES = 3;

        static const char REGISTER_NICK_ACK = 101;
        static const char ADD_BUDDIES_ACK = 102;
        static const char INITIALIZE_UDP = 105;
        static const char INITIALIZE_UDP_ACK = 106;

        static const char BUDDIES_IDS = 150;

        CmdPkg(char* buf);
        CmdPkg(char type, unsigned short len);
        virtual ~CmdPkg();

        char getType();
        unsigned short getLen();
        char* getData();
        char* getBytes();
        void setInt(int data_index, int val);
        void setShort(int data_index, short val);
        void setBytes(int data_index, char* buf, int len);

    private:
        char* bytes;

};

#endif // CMDPKG_H
