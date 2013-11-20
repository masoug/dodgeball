/*
 * Manages all network operations for dodgeball
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <string>
#include <queue>

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
    "A player already has that name.",
    "Game has already started."
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
        bool isStateDirty();
        
        /* event queue handler */
        NetProtocol::FieldEvent* releaseLatestFieldEvent();
        unsigned int eventSize(); // number of events in queue

    protected:
        bool m_run, m_isStateDirty;
        ENetAddress  m_address;
        ENetHost    *m_enetHost;
        ENetEvent    m_event;
        NetProtocol::GameState *m_gameState;
        OpenThreads::Mutex  m_dataLock;
        OpenThreads::Mutex  m_hostLock;
        std::queue<NetProtocol::FieldEvent*> m_fieldEvents;
        OpenThreads::Mutex  m_fieldEventLock;
};

class DodgeballServer : public NetBase {
    public:
        DodgeballServer(unsigned short port);
        virtual ~DodgeballServer();
        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        void sendGameState();
        void broadcastToEveryoneElse(
            ENetPeer *except, NetProtocol::NetPacket &packet,
            unsigned int channel);
        void updateGameState(NetProtocol::GameState *state);
        
        /* handlers */
        void hdlPlayerRequest(
            NetProtocol::PlayerRequest *playerRequest);
        void hdlFieldEvent(
            NetProtocol::FieldEvent *fevent);
        
    protected:
        bool m_gameStarted;
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
        void sendPlayerEvent(
            unsigned int playerID,
            double vel_x, double vel_y, double vel_z);
        void sendSpawnBall(
            unsigned int ballID,
            double pos_x, double pos_y, double pos_z,
            double imp_x, double imp_y, double imp_z);

        /* handlers */
        void hdlFieldEvent(
            NetProtocol::FieldEvent *fevent);

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
