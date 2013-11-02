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

NetBase::NetBase() : m_run(true) {
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
        int status = enet_host_service(m_enetHost, &m_event, 3);
        if (status > 0) {
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
        } else if (status < 0) {
            /* ERROR Occurred */
        } else {
            /* Nothing */
        }
    }
}

void NetBase::sendError(ENetPeer *peer, unsigned int errCode) {
    NetProtocol::Error *error = new NetProtocol::Error;
    error->set_errorcode(errCode);
    error->set_errormsg(NET_ERROR_CODES[errCode]);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::ERROR);
    packet.set_allocated_error(error);
    sendPacket(packet, peer, 0);
}

void NetBase::sendPacket(
    NetProtocol::NetPacket &packet, ENetPeer *peer, unsigned int channel) {
    unsigned int pktSize = packet.ByteSize();
    char buffer[pktSize];
    packet.SerializeToArray(&buffer, pktSize);

    ENetPacket *enetPkt = enet_packet_create(
        &buffer, pktSize, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, channel, enetPkt);
}

NetBase::~NetBase() {
    enet_host_destroy(m_enetHost);
    enet_deinitialize();
}

DodgeballServer::DodgeballServer(unsigned short port) {
    /* Set address */
    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_enetHost = enet_host_create(&m_address, 6, 2, 0, 0);
    if (m_enetHost == NULL) {
        setState(FATAL_STATE);
        m_run = false;
        return;
    }
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
            hdlPlayerRequest(netPacket.player_request());
            break;
        default:
            break;
    }
    enet_packet_destroy(m_event.packet);
}

void DodgeballServer::onDisconnect() {
    NetBase::onDisconnect();
}

DodgeballServer::~DodgeballServer() {
    /* dtor */
}

DodgeballClient::DodgeballClient() : 
    m_connected(false)
{
    m_enetHost = enet_host_create(NULL, 1, 2, 0, 0);
    if (m_enetHost == NULL) {
        setState(FATAL_STATE);
        m_run = false;
        return;
    }
}

void DodgeballServer::hdlPlayerRequest(
    const NetProtocol::PlayerRequest &playerRequest) {
    std::cout << "Player request: " << 
        playerRequest.DebugString() << std::endl;

    /* construct a reponse */
    /* already six players? */
    if (m_gameState.player_state_size() >= 0) {
        sendError(m_event.peer, 0);
        return;
    }
    /* Check for duplicate names */
    for (int i = 0; i < m_gameState.player_state_size(); i++) {
        if (m_gameState.player_state(i).name() == 
            playerRequest.name())
        {
            sendError(m_event.peer, 1);
            return;
        }
    }
    /* authorize new player */
    /* Add new player state to game state */
    NetProtocol::PlayerState *newPlayer = m_gameState.add_player_state();
    newPlayer->set_id(m_gameState.player_state_size());
    newPlayer->set_allocated_position(NewVector3(0.0, 0.0, 0.0));
    newPlayer->set_possesion(3);
    newPlayer->set_allocated_targetvelocity(NewVector3(0.0, 0.0, 0.0));
    newPlayer->set_avatar(0);
    newPlayer->set_name(playerRequest.name());
    /* Send player confirmation */
    NetProtocol::PlayerConfirmation *confirmation = 
        new NetProtocol::PlayerConfirmation;
    confirmation->set_id(m_gameState.player_state_size());

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::PLAYER_CONFIRMATION);
    packet.set_allocated_player_confirmation(confirmation);
    sendPacket(packet, m_event.peer, 0);
}

DodgeballClient::~DodgeballClient() {
    /* dtor */
}

bool DodgeballClient::connect(std::string address, unsigned short port) {
    enet_address_set_host(&m_address, address.c_str());
    m_address.port = port;

    /* Make the connection! */
    m_server = enet_host_connect(m_enetHost, &m_address, 2, 0);
    if (m_server == NULL) {
        std::cerr << 
            "No available peers to initiate ENET connection!" << std::endl;
        return false;
    }

    /* Wait for up to 5 seconds to connection attempt to succeed. */
    if (enet_host_service(m_enetHost, &m_event, 5000) > 0 &&
        m_event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connection succeeded!" << std::endl;
        return true;
    } else {
        std::cerr << "Connection timed out!" << std::endl;
        std::cerr << "Try restarting the program." << std::endl;
        return false;
    }
    return false;
}

bool DodgeballClient::isConnected() const {
    if (m_server == NULL)
        return false;
    else {
        if (m_connected)
            return true;
        else
            return false;
    }
}

bool DodgeballClient::playerRequest(std::string username) {
    /* Send a player request */
    NetProtocol::PlayerRequest *playerReq = 
        new NetProtocol::PlayerRequest();
    playerReq->set_name(username);

    NetProtocol::NetPacket packet;
    packet.set_type(NetProtocol::NetPacket::PLAYER_REQUEST);
    packet.set_allocated_player_request(playerReq);
    sendPacket(packet, m_server, 0);

    /* wait for up to 3 seconds for the server's response. */
    if (enet_host_service(m_enetHost, &m_event, 3000) > 0 &&
        m_event.type == ENET_EVENT_TYPE_RECEIVE)
    {
        std::cout << "Server response:" << std::endl;
        NetProtocol::NetPacket response;
        if (!response.ParseFromArray(
            m_event.packet->data, m_event.packet->dataLength))
        {
            std::cerr << "Malformed packet!" << std::endl;
            enet_packet_destroy(m_event.packet);
            return false;
        }
        
        /* handle possible responses from server */
        switch (response.type()) {
            case NetProtocol::NetPacket::PLAYER_CONFIRMATION:
                m_playerID = response.player_confirmation().id();
                break;
            case NetProtocol::NetPacket::ERROR:
                std::cout << "ERROR: " << 
                    response.error().DebugString() << std::endl;
                return false;
                break;
            default:
                return false;
                break;
        }
        enet_packet_destroy(m_event.packet);
        return true;
    } else {
        std::cerr << "Player request timeout!" << std::endl;
        std::cerr << "Try restarting the program." << std::endl;
        return false;
    }
    return false;
}

void DodgeballClient::onConnect() {
    NetBase::onConnect();
    m_connected = true;
}

void DodgeballClient::onDisconnect() {
    NetBase::onDisconnect();
    m_connected = false;
}

void DodgeballClient::onReceive() {
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

int DodgeballClient::getPlayerID() const {
    return m_playerID;
}

