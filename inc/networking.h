/*
 * Manages all network operations for dodgeball
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <string>

#include <enet/enet.h>
#include <Thread>
#include <Mutex>

#include "state_machine.h"
#include "messages.pb.h"

/* Protocol error codes */
const std::string NET_ERROR_CODES[] = {
    "All player slots are filled.",
    "A player already has that name."
};

/* Convenience methods */
NetProtocol::Vector3* newVector3(double x, double y, double z) {
    NetProtocol::Vector3 *result = new NetProtocol::Vector3;
    result->set_x(x);
    result->set_y(y);
    result->set_z(z);
    return result;
}

class NetBase : public OpenThreads::Thread, public StateMachineBase {
    public:
        NetBase();
        virtual ~NetBase();

        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        void run();
        virtual void gracefulStop() { m_run = false; join(); }
        virtual void sendError(ENetPeer *peer, unsigned int errCode);
        virtual void sendPacket(
            NetProtocol::NetPacket &packet, ENetPeer *peer,
            unsigned int channel);

    protected:
        bool m_run;
        ENetAddress  m_address;
        ENetHost    *m_enetHost;
        ENetEvent    m_event;
};

class DodgeballServer : public NetBase {
    public:
        DodgeballServer(unsigned short port);
        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        
        /* handlers */
        void hdlPlayerRequest(
            const NetProtocol::PlayerRequest &playerRequest);
        
        virtual ~DodgeballServer();

    private:
        /* This is the *master* game state! */
        NetProtocol::GameState m_gameState;
};

class DodgeballClient : public NetBase {
    public:
        DodgeballClient();
        virtual ~DodgeballClient();

        bool connect(std::string address, unsigned short port);
        bool isConnected() const;
        bool playerRequest(std::string username);
        virtual void onConnect();
        virtual void onDisconnect();
        virtual void onReceive();
        void gracefulStop();

    protected:
        bool m_connected;
        ENetPeer *m_server = NULL;
};

#endif
