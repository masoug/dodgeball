/*
 * Manages all network operations for dodgeball
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <string>

#include <enet/enet.h>
#include <Thread>
#include <Mutex>
#include <ScopedLock>

#include "state_machine.h"
#include "messages.pb.h"
#include "dynamic_object.h"

/* Thread locking mechanism */
#define SLOCK(_mutex) OpenThreads::ScopedLock<OpenThreads::Mutex> __lock(_mutex);

/* Protocol error codes */
const std::string NET_ERROR_CODES[] = {
    "All player slots are filled.",
    "A player already has that name."
};

/* Convenience methods */
NetProtocol::Vector3* NewVector3(double x, double y, double z);

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
        
        /* getters and setters */
        NetProtocol::GameState getGameState();

    protected:
        bool m_run;
        ENetAddress  m_address;
        ENetHost    *m_enetHost;
        ENetEvent    m_event;
        NetProtocol::GameState *m_gameState;
        OpenThreads::Mutex  m_dataLock;
        OpenThreads::Mutex  m_hostLock;
};

class DodgeballServer : public NetBase {
    public:
        DodgeballServer(unsigned short port);
        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        void sendGameState();
        
        /* handlers */
        void hdlPlayerRequest(
            const NetProtocol::PlayerRequest &playerRequest);
        
        virtual ~DodgeballServer();
};

class DodgeballClient : public NetBase {
    public:
        DodgeballClient();
        virtual ~DodgeballClient();

        bool connect(std::string address, unsigned short port);
        bool isConnected();
        void playerRequest(std::string username);
        virtual void onConnect();
        virtual void onDisconnect();
        virtual void onReceive();
        void gracefulStop();

        /* yielding calls */
        bool waitForConnection(unsigned int timeout_ms);
        bool waitForInitialState();
        bool waitForConfirmation(unsigned int timeout_ms);
        
        /* getter/setter functions */
        int getPlayerID();

    protected:
        bool m_connected;
        int m_playerID;
        ENetPeer *m_server = NULL;
        NetProtocol::Error *m_errorPacket;
};

#endif
