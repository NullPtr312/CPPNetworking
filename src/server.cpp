// server.cpp
#include "../include/enet/enet.h"
#include <iostream>

int main(int argc, const char **argv)
{
    if (enet_initialize() != 0)
    {
        std::cerr << "Failed to initialize ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY; // Listen on all interfaces
    address.port = 1234;

    ENetHost *server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server)
    {
        std::cerr << "Failed to create ENet server.\n";
        return EXIT_FAILURE;
    }

    std::cout << "Server started on port 1234...\n";

    ENetEvent event;
    while (true)
    {
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "A new client connected from "
                          << (event.peer->address.host & 0xFF) << "."
                          << ((event.peer->address.host >> 8) & 0xFF) << "."
                          << ((event.peer->address.host >> 16) & 0xFF) << "."
                          << ((event.peer->address.host >> 24) & 0xFF) << ":"
                          << event.peer->address.port << "\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE: 
                std::cout << "Message: " << event.packet->data << "\n";
                // Broadcast to all peers except original sender

                for (int i = 0; i < server->peerCount; i++)
                {
                    ENetPeer* peer = server->peers + i;
                    if (peer->state == ENET_PEER_STATE_CONNECTED && peer != event.peer) {
                        ENetPacket* packetCopy = enet_packet_create(event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
                        enet_peer_send(peer, 0, packetCopy);
                    }
                }

                enet_host_flush(server);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected.\n";
                break;

            default:
                break;
            }
        }
    }

    enet_host_destroy(server);
    return 0;
}
