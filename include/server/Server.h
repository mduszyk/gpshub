#ifndef SERVER_H
#define SERVER_H

/**
 * Server interface
 */
class Server
{
    public:
        // server loop
        virtual void loop() = 0;
        virtual void stop() = 0;
};

#endif // SERVER_H
