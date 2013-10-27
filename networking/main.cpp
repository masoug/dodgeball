#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>

#include <enet/enet.h>
#include <Thread>

#include <messages.pb.h>

using namespace std;

struct Player {
    char *username;
    int points;
};

class Server : public OpenThreads::Thread {
    public:
        Server(unsigned short port);
        virtual ~Server();
        
        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        virtual void run();
        void shutdown() { m_run = false; }

        /* handlers */
        virtual void hdlPlayerReq(
            NetProtocol::RequestPlayer playerReq, ENetPeer *peer);

    private:
        bool m_run;
        ENetAddress m_address;
        ENetHost *m_host;
        ENetEvent m_event;
};

Server::Server(unsigned short port) : m_run(true) {
    /* server */
    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    if (enet_initialize() != 0)
        return;
    
    m_host = enet_host_create(&m_address, 6, 2, 0, 0);
    if (m_host == NULL)
        return;
}

void Server::onConnect() {
    printf("New player from %x:%u\n",
        m_event.peer->address.host,
        m_event.peer->address.port);

    m_event.peer->data = (Player*)malloc(sizeof(Player));
}

void Server::onReceive() {
    /* Attempt to deserialize the packet */
    NetProtocol::NetPacket  netPacket;
    if (!netPacket.ParseFromArray(
            m_event.packet->data, m_event.packet->dataLength))
    {
        enet_packet_destroy(m_event.packet);
        return;
    }

    switch (netPacket.type()) {
        case NetProtocol::NetPacket::REQPLAYER:
            cout << "Player request => " << 
                netPacket.request_player().name() << endl;
                hdlPlayerReq(netPacket.request_player(), m_event.peer);
            break;
        default:
            break;
    }

    enet_packet_destroy(m_event.packet);
}

void Server::onDisconnect() {
    free(m_event.peer->data);
    m_event.peer->data = NULL;
}

void Server::run() {
    /* run */
    while (m_run) {
        int hostStatus = enet_host_service(m_host, &m_event, 3);
        if (hostStatus > 0) {
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
        } else if (hostStatus < 0) {
            /* Error occurred */
        } else {
            /* Nothing */
        }
    }
}

Server::~Server() {
    enet_host_destroy(m_host);
    enet_deinitialize();
}

void Server::hdlPlayerReq(
            NetProtocol::RequestPlayer playerReq, ENetPeer *peer)
{
    /* Create player response */
    NetProtocol::PlayerConfirmation response;
    response.set_id(m_host->peerCount);
    response.set_name(playerReq.name());
    response.set_type(0);
    
    /* Set initial position */
    NetProtocol::Vector3 initPos;
    initPos.set_x(0.0);
    initPos.set_y(0.0);
    initPos.set_z(0.0);
    response.set_allocated_initpos(&initPos);

    /* Create the packet and send! */
    NetProtocol::NetPacket netPacket;
    netPacket.set_type(NetProtocol::NetPacket::PLAYERCONFIRM);
    netPacket.set_allocated_player_confirmation(&response);
    int pktSize = netPacket.ByteSize();
    char buffer[pktSize];
    netPacket.SerializeToArray(&buffer, pktSize);
    ENetPacket *enetPacket = enet_packet_create(
        &buffer, pktSize, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, enetPacket);

    /* setup the player struct */
    Player *player = (Player*)peer->data;
    player->points = 0;
    strcpy(player->username, playerReq.name().c_str());
}

class Client : OpenThreads::Thread {
    public:
        Client(string server, unsigned short port, string username);
        virtual ~Client();
        
        virtual void onConnect();
        virtual void onReceive();
        virtual void onDisconnect();
        virtual void run();
        void shutdown() { m_run = false; }

    private:
        string m_username;
        bool m_run;
        ENetAddress m_address;
        ENetHost *m_host;
        ENetEvent m_event;
        ENetPeer *m_server;
};

Client::Client(string server, unsigned short port, string username) : 
    m_username(username), m_run(true)
{
    if (enet_initialize() != 0)
        return;

    enet_address_set_host(&m_address, server.c_str());
    m_address.port = port;

    m_host = enet_host_create(NULL, 1, 2, 0, 0);
    if (m_host == NULL)
        return;
}

void Client::onConnect() {
    /* Send player request to server */
    NetProtocol::RequestPlayer playerReq;
    playerReq.set_name(m_username);

    NetProtocol::NetPacket netPacket;
    netPacket.set_type(NetProtocol::NetPacket::REQPLAYER);
    netPacket.set_allocated_request_player(&playerReq);

    int pktSize = netPacket.ByteSize();
    char buffer[pktSize];
    netPacket.SerializeToArray(&buffer, pktSize);
    ENetPacket *packet = enet_packet_create(
        &buffer, pktSize, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(m_server, 0, packet);
}

void Client::onReceive() {
    /* Attempt to deserialize packet */
    NetProtocol::NetPacket netPacket;
    if (!netPacket.ParseFromArray(
            m_event.packet->data, m_event.packet->dataLength))
    {
        enet_packet_destroy(m_event.packet);
        return;
    }
    
    switch (netPacket.type()) {
        case NetProtocol::NetPacket::PLAYERCONFIRM:
            cout << "I'm confirmed!" << endl;
            break;
        default:
            break;
    }

    enet_packet_destroy(m_event.packet);
}

void Client::onDisconnect() {
}

void Client::run() {
    /* Initiate connection to server */
    m_server = enet_host_connect(m_host, &m_address, 2, 0);
    if (m_server == NULL)
        return;
    
    while (m_run) {
        int hostStatus = enet_host_service(m_host, &m_event, 3);
        if (hostStatus > 0) {
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
        } else if (hostStatus < 0) {
            /* Error occurred */
        } else {
            /* Nothing */
        }
    }
}

Client::~Client() {
    enet_peer_disconnect(m_server, 0);
    enet_host_destroy(m_host);
    enet_deinitialize();
}

int main(int argc, char *argv[]) {
    cout << "N E T W O R K I N G" << endl;

    if (argc != 2) {
        cerr << argv[0] << " [client/server]" << endl;
        return EXIT_FAILURE;
    }
    

    bool isClient = false;
    if (strcmp(argv[1], "client") == 0)
        isClient = true;
    else if (strcmp(argv[1], "server") == 0)
        isClient = false;
    else {
        cerr << "Set me as either client or server!" << endl;
        return EXIT_FAILURE;
    }

    if (enet_initialize() != 0) {
        cerr << "Error starting enet!" << endl;
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost    *enetHost;

    /* Setup the address */
    if (isClient) {
        enet_address_set_host(&address, "localhost");
    } else {
        address.host = ENET_HOST_ANY;
    }
    address.port = 7654;

    /* Setup the host */
    if (isClient)
        enetHost = enet_host_create(NULL, 1, 2, 0, 0);
    else 
        enetHost = enet_host_create(&address, 6, 2, 0, 0);
    if (enetHost == NULL) {
        cerr << "Error creating enet host!" << endl;
        return EXIT_FAILURE;
    }

    ENetEvent   event;
    ENetPeer    *peer;
    
    if (isClient) {
        peer = enet_host_connect(enetHost, &address, 2, 0);
        if (peer == NULL) {
            cerr << "No peers to connect to!" << endl;
            return EXIT_FAILURE;
        }

        if (enet_host_service(enetHost, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
            cout << "Sending stuff to server" << endl;
            enet_host_flush(enetHost);
            string input;
            cin >> input;
            while (input != "quit") {
                ENetPacket *packet = enet_packet_create(
                    input.c_str(), strlen(input.c_str())+1, ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
                enet_host_flush(enetHost);
                cin >> input;
            }
            enet_peer_disconnect(peer, 0);
            enet_host_flush(enetHost);
        } else {
            enet_peer_reset(peer);
        }
    } else {
        cout << "SERVERMODE" << endl;
        while (true) {
            while (enet_host_service(enetHost, &event, 10) > 0) {
                /* Handle stuff */
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                        cout << "Why hello there "
                            << event.peer->address.host << ":"
                            << event.peer->address.port << endl;
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        cout << "Thanks man!" << endl
                        << "Packet: " << event.packet->data 
                            << " (" << event.packet->dataLength << ")"<< endl
                        << "Channel: " << (int)event.channelID << endl;

                        enet_packet_destroy(event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        cout << "Bye bye " << event.peer << endl;
                        event.peer->data = NULL;
                        break;
                    default:
                        cout << "HARO" << endl;
                        break;
                }
            }
        }
    }
    
    enet_host_destroy(enetHost);
    atexit(enet_deinitialize);

    return EXIT_SUCCESS;
}
