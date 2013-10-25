#include <iostream>
#include <enet/enet.h>

using namespace std;

int main(int argc, char *argv[]) {
    cout << "N E T W O R K I N G" << endl;

    if (enet_initialize() != 0) {
        cerr << "Error starting enet!" << endl;
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost    *server;

    /* Setup the address */
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    /* Setup the host */
    server = enet_host_create(&address, 6, 2, 0, 0);
    if (server == NULL) {
        cerr << "Error creating enet host!" << endl;
        return EXIT_FAILURE;
    }

    ENetEvent event;

    while (enet_host_service(server, &event, 1000) > 0) {
        /* Handle stuff */
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                break;
        }
    }
    
    enet_host_destroy(server);
    atexit(enet_deinitialize);

    return 0;
}
