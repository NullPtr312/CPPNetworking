// client.cpp
#include "..\include\enet\enet.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

void inputThread(ENetPeer* peer)
{
    while (running)
    {
        std::string msg;
        if (std::getline(std::cin, msg))
        {
            if (msg == "q") {
                running = false;
                break;
            }
            ENetPacket *packet = enet_packet_create(
                msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            enet_host_flush(peer->host);
        }
    }
}



int main(int argc, const char **argv)
{
    if (enet_initialize() != 0)
    {
        std::cerr << "Failed to initialize ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost *client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client)
    {
        std::cerr << "Failed to create ENet client.\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;

    std::string ip_address;
    std::cout << "Connect to what IP: ";
    std::cin >> ip_address; 
    enet_address_set_host(&address, ip_address.c_str());
    address.port = 1234;

    ENetPeer *peer = enet_host_connect(client, &address, 2, 0);
    if (!peer)
    {
        std::cerr << "No available peers for initiating connection.\n";
        return EXIT_FAILURE;
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connected to server!\n";
    }
    else
    {
        std::cerr << "Connection failed.\n";
        enet_peer_reset(peer);
        return EXIT_FAILURE;
    }


    // Main loop

    std::thread t(inputThread, peer);

    while (running)
    {
        // Handle incoming packets
        while (enet_host_service(client, &event, 100) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "[Server] " << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected\n";
                running = false;
                break;
            default:
                break;
            }
        }

        // Handle user input
        
    }

    if (t.joinable()) t.join();
    enet_host_destroy(client);
    return 0;
}
