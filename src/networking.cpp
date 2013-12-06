#include <iostream>
#include <cstdio>

#include "networking.h"

/* Convenience methods */
NetProtocol::Vector3* NewVector3(double x, double y, double z) {
    NetProtocol::Vector3 *result = new NetProtocol::Vector3;
    result->set_x(x);
    result->set_y(y);
    result->set_z(z);
    return result;
}

NetBase::NetBase() : m_run(true), m_isStateDirty(false) {
    /* stop thred from running */
    if (enet_initialize() != 0) {
        m_run = false;
        setState(FATAL_STATE);
        return;
    }
    
    setState(INIT_STATE);
}

void NetBase::onConnect() {
    std::cout << "CONNECT" << std::endl;
}

void NetBase::onReceive() {
    std::cout << "RECEIVED" << std::endl;
}

void NetBase::onDisconnect() {
    std::cout << "DISCONNECTED" << std::endl;
}

void NetBase::run() {
    std::cout << "ENET run()" << std::endl;
    while (m_run) {
        SLOCK(m_hostLock)
        while (enet_host_service(m_enetHost, &m_event, 3) > 0) {
            /* handle events */
            switch (m_event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    onConnect();
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    onReceive();
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    onDisconnect();
                    break;
                default:
                    break;
            }
            setState(NTWK_ACTIVE);
        } 
    }
}

void NetBase::sendError(ENetPeer *peer, unsigned int errCode) {
    NetProtocol::Error *error = new NetProtocol::Error;
    error->set_errorcode(errCode);
    error->set_errormsg(NET_ERROR_CODES[errCode]);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::ERROR_PKT);
    packet.set_allocated_error(error);
    sendPacket(packet, peer, 0);
}

void NetBase::sendPacket(
    NetProtocol::NetPacket &packet, ENetPeer *peer, unsigned int channel)
{
    unsigned int pktSize = packet.ByteSize();
    uint8_t buffer[pktSize];
    packet.SerializeToArray(&buffer, pktSize);

    ENetPacket *enetPkt = enet_packet_create(
        &buffer, pktSize, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, channel, enetPkt);
}

NetProtocol::GameState NetBase::getGameState() {
    SLOCK(m_dataLock)
    if (!m_gameState)
        return NetProtocol::GameState();
    /* QUESTION: is this the same?
     * return *m_gameState;
     */
    m_isStateDirty = false;
    return NetProtocol::GameState(*m_gameState);
}

bool NetBase::isStateDirty() {
    SLOCK(m_dataLock)
    return m_isStateDirty;
}

NetProtocol::FieldEvent* NetBase::releaseLatestFieldEvent() {
    SLOCK(m_fieldEventLock)
    NetProtocol::FieldEvent* fe = m_fieldEvents.front();
    m_fieldEvents.pop();
    return fe;
}

unsigned int NetBase::eventSize() {
    SLOCK(m_fieldEventLock)
    return m_fieldEvents.size();
}

NetBase::~NetBase() {
    enet_host_destroy(m_enetHost);
    enet_deinitialize();
}

/*
 * DODGEBALL SERVER
 */

DodgeballServer::DodgeballServer(unsigned short port) :
    m_gameStarted(false)
{
    /* Set address */
    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_enetHost = enet_host_create(&m_address, 6, 2, 0, 0);
    if (m_enetHost == NULL) {
        setState(FATAL_STATE);
        m_run = false;
        return;
    }

    m_gameState = new NetProtocol::GameState;
    m_gameState->set_redpoints(100);
    m_gameState->set_bluepoints(100);

    start();
}

DodgeballServer::~DodgeballServer() {
    /* dtor */
}

void DodgeballServer::onConnect() {
    printf("Hello there %x:%u!\n",
        m_event.peer->address.host,
        m_event.peer->address.port);
}

void DodgeballServer::onReceive() {
    /* Deserialize packet */
    NetProtocol::NetPacket netPacket;
    if (!netPacket.ParseFromArray(
            m_event.packet->data, m_event.packet->dataLength))
    {
        enet_packet_destroy(m_event.packet);
        return;
    }

    switch (netPacket.type()) {
        case NetProtocol::NetPacket::PLAYER_REQUEST:
            hdlPlayerRequest(netPacket.release_player_request());
            break;
        case NetProtocol::NetPacket::FIELD_EVENT:
            hdlFieldEvent(netPacket.release_field_event());
        default:
            break;
    }
    enet_packet_destroy(m_event.packet);
}

void DodgeballServer::onDisconnect() {
    NetBase::onDisconnect();
}

void DodgeballServer::sendGameState() {
    /* Serialize game state */
    SLOCK(m_dataLock)
    NetProtocol::GameState *snapshot = new NetProtocol::GameState(*m_gameState);
    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::GAME_STATE);
    packet.set_allocated_game_state(snapshot);

    broadcastToEveryoneElse(NULL, packet, 1);
    m_gameStarted = true;
}

void DodgeballServer::broadcastToEveryoneElse(
    ENetPeer *except, NetProtocol::NetPacket &packet, unsigned int channel)
{
    /* broadcasts the packet to everyone except `except`.
     * if except is null, the packet is broadcast to everyone */
    for (unsigned int i = 0; i < m_enetHost->peerCount; i++) {
        if (except != &m_enetHost->peers[i])
            sendPacket(packet, &m_enetHost->peers[i], channel);
    }
}

void DodgeballServer::updateGameState(NetProtocol::GameState *state) {
    /* overwrite our own version of the new version */
    m_dataLock.lock();
    if (!m_gameState) {
        m_dataLock.unlock();
        return;
    }

    delete m_gameState;
    m_gameState = state;
    m_isStateDirty = true;
    m_dataLock.unlock();

    /* send the game state */
    sendGameState();
}

void DodgeballServer::broadcastRepossession(unsigned int ballID, unsigned int playerID) {
    NetProtocol::BallRepossessed *brp = new NetProtocol::BallRepossessed;
    brp->set_playerid(playerID);
    brp->set_ballid(ballID);

    NetProtocol::FieldEvent *fevent = new NetProtocol::FieldEvent;
    fevent->set_type(NetProtocol::FieldEvent::BALL_REPOSSESSED);
    fevent->set_allocated_ball_repossessed(brp);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::FIELD_EVENT);
    packet.set_allocated_field_event(fevent);

    broadcastToEveryoneElse(NULL, packet, 0);
}

void DodgeballServer::hdlPlayerRequest(
    NetProtocol::PlayerRequest *playerRequest) {
    std::cout << "Player request: " << 
        playerRequest->DebugString() << std::endl;

    /* construct a reponse */
    /* game already started? */
    if (m_gameStarted) {
        sendError(m_event.peer, 2);
        return;
    }
    /* already six players? */
    SLOCK(m_dataLock)
    if (m_gameState->player_state_size() >= 3) {
        sendError(m_event.peer, 0);
        return;
    }
    /* Check for duplicate names */
    for (int i = 0; i < m_gameState->player_state_size(); i++) {
        if (m_gameState->player_state(i).name() == 
            playerRequest->name())
        {
            sendError(m_event.peer, 1);
            return;
        }
    }
    
    /* authorize new player */
    /* Add new player state to game state */
    NetProtocol::PlayerState *newPlayer = m_gameState->add_player_state();
    newPlayer->set_name(playerRequest->name());
    newPlayer->set_possession(3);
    newPlayer->set_allocated_targetvelocity(NewVector3(0.0, 0.0, 0.0));
    
    int playerID = m_gameState->player_state_size();
    newPlayer->set_id(playerID);
    newPlayer->set_avatar(playerID-1);

    /* assign team to player */
    if (playerID == 2)
        newPlayer->set_team_type(AvatarNode::RED);
    else
        newPlayer->set_team_type(AvatarNode::BLUE);
    
    /* assign initial position on the court. */
    if (newPlayer->team_type() == AvatarNode::BLUE) {
        newPlayer->set_allocated_position(
            NewVector3(2.5*((playerID-1)%3) - 2.5, 1.0, 2.5));
    } else if (newPlayer->team_type() == AvatarNode::RED) {
        newPlayer->set_allocated_position(
            NewVector3(2.5*((playerID-1)%3) - 2.5, 1.0, -2.5));
    } else {
        /* whaaat? */
        std::cerr << "Awkward exception :( "
            << "New player is neither red or blue!" << std::endl;
        newPlayer->set_allocated_position(NewVector3(0.0, 1.0, 0.0));
    }

    /* Send player confirmation */
    NetProtocol::PlayerConfirmation *confirmation = 
        new NetProtocol::PlayerConfirmation;
    confirmation->set_id(playerID);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::PLAYER_CONFIRMATION);
    packet.set_allocated_player_confirmation(confirmation);
    sendPacket(packet, m_event.peer, 0);

    std::cout << "\n\nCONFIRMED PLAYER "
        << newPlayer->DebugString() << "\n\n" << std::endl;
    m_isStateDirty = true;
}

void DodgeballServer::hdlFieldEvent(
    NetProtocol::FieldEvent *fevent)
{
    std::cout << fevent->DebugString() << std::endl;
    /* broadcast field event to everyone else
     * Maybe a more efficient method to do this?
     */
    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::FIELD_EVENT);
    packet.set_allocated_field_event(new NetProtocol::FieldEvent(*fevent));

    broadcastToEveryoneElse(m_event.peer, packet, 0); 
    /* push event to event queue for the physics engine
     * to pick up on the render thread.
     */
    SLOCK(m_fieldEventLock)
    if (m_fieldEvents.size() >= 1000) {
        std::cerr << "EVENT QUEUE OVERFLOW!!! WTF!?!!?!?!?" << std::endl;
        return;
    }
    m_fieldEvents.push(fevent);
}

/*
 * DODGEBALL CLIENT
 */

DodgeballClient::DodgeballClient() : 
    m_connected(false), m_playerID(-1)
{
    m_enetHost = enet_host_create(NULL, 1, 2, 0, 0);
    if (m_enetHost == NULL) {
        setState(FATAL_STATE);
        m_run = false;
        return;
    }

    m_gameState = NULL;
    m_errorPacket = NULL;

    start();
}

DodgeballClient::~DodgeballClient() {
    /* dtor */
}

bool DodgeballClient::connect(std::string address, unsigned short port) {
    enet_address_set_host(&m_address, address.c_str());
    m_address.port = port;

    /* Make the connection! */
    SLOCK(m_dataLock)
    m_server = enet_host_connect(m_enetHost, &m_address, 2, 0);
    if (m_server == NULL) {
        std::cerr << 
            "No available peers to initiate ENET connection!" << std::endl;
        return false;
    }
    return true;
}

bool DodgeballClient::isConnected() {
    SLOCK(m_dataLock);
    return (m_server != NULL) && m_connected;
}

void DodgeballClient::playerRequest(std::string username) {
    /* Send a player request */
    NetProtocol::PlayerRequest *playerReq = 
        new NetProtocol::PlayerRequest();
    playerReq->set_name(username);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::PLAYER_REQUEST);
    packet.set_allocated_player_request(playerReq);
    sendPacket(packet, m_server, 0);
}

void DodgeballClient::onConnect() {
    NetBase::onConnect();
    
    SLOCK(m_dataLock)
    m_connected = true;
}

void DodgeballClient::onDisconnect() {
    NetBase::onDisconnect();

    SLOCK(m_dataLock)
    m_connected = false;
}

void DodgeballClient::onReceive() {
    NetProtocol::NetPacket response;
    if (!response.ParseFromArray(
        m_event.packet->data, m_event.packet->dataLength))
    {
        std::cerr << "Malformed packet!" << std::endl;
        enet_packet_destroy(m_event.packet);
        return;
    }
    
    /* handle possible responses from server */
    switch (response.type()) {
        case NetProtocol::NetPacket::PLAYER_CONFIRMATION:
        {
            SLOCK(m_dataLock)
            m_playerID = response.player_confirmation().id();
            std::cout << "Player ID: " << m_playerID << std::endl;
        }
            break;
        case NetProtocol::NetPacket::GAME_STATE:
        {
            SLOCK(m_dataLock)
            if (m_gameState) // conserve memory
                free(m_gameState);
            m_gameState = response.release_game_state();
            m_isStateDirty = true;
            //std::cout << "\n\nGAMESTATE:\n"
            //    << m_gameState->DebugString() << std::endl;
        }
            break;
        case NetProtocol::NetPacket::ERROR_PKT:
        {
            SLOCK(m_dataLock);
            m_errorPacket = response.release_error();
        }
            break;
        case NetProtocol::NetPacket::FIELD_EVENT:
            hdlFieldEvent(response.release_field_event());
        default:
            break;
    }
    enet_packet_destroy(m_event.packet);
}

void DodgeballClient::gracefulStop() {
    NetBase::gracefulStop();
    
    enet_peer_disconnect(m_server, 0);
    bool success = false;
    if (enet_host_service(m_enetHost, &m_event, 3000) > 0) {
        switch (m_event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(m_event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "DISCONNECTED" << std::endl;
                success = true;
                break;
            default:
                break;
        }
    }
    if (!success) {
        std::cerr << "Forcefully disconnecting from server!" << std::endl;
        enet_peer_reset(m_server);
    }
    m_server = NULL;
}

void DodgeballClient::sendPlayerEvent(
    unsigned int playerID,
    double vel_x, double vel_y, double vel_z)
{
    NetProtocol::PlayerEvent *ptrAction = new NetProtocol::PlayerEvent;
    ptrAction->set_id(playerID);
    ptrAction->set_allocated_targetvelocity(NewVector3(vel_x, vel_y, vel_z));
    NetProtocol::FieldEvent *fevent = new NetProtocol::FieldEvent;
    fevent->set_type(NetProtocol::FieldEvent::PLAYER_EVENT);
    fevent->set_allocated_player_event(ptrAction);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::FIELD_EVENT);
    packet.set_allocated_field_event(fevent);
    sendPacket(packet, m_server, 0);
}

void DodgeballClient::sendSpawnBall(
    unsigned int ballID, unsigned int playerID,
    double pos_x, double pos_y, double pos_z,
    double imp_x, double imp_y, double imp_z)
{
    NetProtocol::SpawnBall *sball = new NetProtocol::SpawnBall;
    sball->set_id(ballID);
    sball->set_fromid(playerID);
    sball->set_allocated_impulse(NewVector3(imp_x, imp_y, imp_z));
    sball->set_allocated_position(NewVector3(pos_x, pos_y, pos_z));

    NetProtocol::FieldEvent *fevent = new NetProtocol::FieldEvent;
    fevent->set_type(NetProtocol::FieldEvent::SPAWN_BALL);
    fevent->set_allocated_spawn_ball(sball);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::FIELD_EVENT);
    packet.set_allocated_field_event(fevent);
    sendPacket(packet, m_server, 0);
}

void DodgeballClient::hdlFieldEvent(
    NetProtocol::FieldEvent *fevent)
{
    std::cout << fevent->DebugString() << std::endl;
    /* apply to my physics simulation */

    /* push event to event queue for the physics engine
     * to pick up on the render thread.
     */
    SLOCK(m_fieldEventLock)
    if (m_fieldEvents.size() >= 1000) {
        std::cerr << "EVENT QUEUE OVERFLOW!!! WTF!?!!?!?!?" << std::endl;
        return;
    }
    m_fieldEvents.push(fevent);
}

bool DodgeballClient::waitForInitialState() {
    /* wait for server to send initial game state */
    while (true) {
        OpenThreads::Thread::microSleep(1000);

        SLOCK(m_dataLock)
        if (m_gameState)
            return true;
        if (m_errorPacket) {
            std::cerr << "Error occurred during wait for game start:\n"
                << m_errorPacket->DebugString() << std::endl;
            delete m_errorPacket;
            m_errorPacket = NULL;
            return false;
        }
    }
    return false; // should never get here...
}

bool DodgeballClient::waitForConnection(unsigned int timeout_ms) {
    /* wait for confirmation of connection */
    unsigned int timeout = 0;
    while (!isConnected()) {
        if (timeout >= timeout_ms)
            return false;
        OpenThreads::Thread::microSleep(1000);
        timeout++;
    }
    return true;
}

bool DodgeballClient::waitForConfirmation(unsigned int timeout_ms) {
    /* Wait for confirmation of valid player */
    unsigned int timeout = 0;
    while (getPlayerID() < 0) {
        if (timeout >= timeout_ms)
            return false;
        OpenThreads::Thread::microSleep(1000);

        /* If the error packet gets "dirty" in the meantime,
         * process it.
         */
        SLOCK(m_dataLock)
        if (m_errorPacket) {
            std::cerr << "Error occurred while waiting "
                << "for player confirmation:\n"
                << m_errorPacket->DebugString() << std::endl;
            delete m_errorPacket;
            m_errorPacket = NULL;
            return false;
        }
        timeout++;
    }
    return true;
}

int DodgeballClient::getPlayerID() {
    SLOCK(m_dataLock)
    return m_playerID;
}


