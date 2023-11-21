#include <SFML/Network.hpp>
#include <PacketFactory.h>
#include <PlayerEntity.h>
#include <TickClock.h>
#include <ConnectedSocket.h>
#include <iostream>
#include <unordered_map>
#include <format>
#include "Server.h"

#define DEBUG_CLONE

std::uint16_t GetNewID(){
    static std::uint16_t usedEntityIds{0};
    usedEntityIds++;
    return usedEntityIds - 1;
}

std::unordered_map<std::uint16_t, ConnectedSocket> clients;
std::unordered_map<std::uint16_t, PlayerEntity> players;
std::vector<sf::Vector2i> barriers;

void ServerMessage(const std::string& message) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    for (auto& [key, value] : clients) {
        value.Send(joinMessage);
    }
    std::cout << message << std::endl;
}

void CheckForNewClient(sf::TcpListener& listener) {
    ConnectedSocket socket{listener};
    if(!socket.IsConnected()) {
        return;
    }
    std::uint16_t id{ GetNewID() };
    ConnectedSocket& client = clients.insert({ id, std::move(socket) }).first->second;
    PlayerEntity& newPlayer = players.insert({ id, PlayerEntity{id, {0.0f, 0.0f}, 0.0f} }).first->second;
    sf::Packet packet{ PacketFactory::JoinGame(newPlayer.id, newPlayer.getPosition(), newPlayer.getRotation(), barriers) };
    client.Send(packet);
    ServerMessage(std::format("A new player ({}) has joined the game", id));
}

void CheckForDisconnectedClients() {
    for (auto& [id, client] : clients) {
        if(!client.IsConnected()) {
            auto idCopy = id;
            players.erase(idCopy);
            clients.erase(idCopy);
            ServerMessage(std::format("A player ({}) has disconnected from the game", idCopy));
            break;
        }
    }
}

void main()
{
    for (int i = 0; i < 16; i++) {
        barriers.push_back({ i - 8,-6 });
        barriers.push_back({ i - 8, 5 });
    }
    for (int i = 0; i < 10; i++) {
        barriers.push_back({ -8,i-5 });
        barriers.push_back({ 7, i-5 });
    }
    for (int i = 0; i < 6; i++) {
        barriers.push_back({ -5,i - 3 });
        barriers.push_back({ 4, i - 3 });
    }
    for (int i = 0; i < 6; i++) {
        barriers.push_back({ -i+1,2 });
        barriers.push_back({ i-2, -3 });
    }

    sf::TcpListener listener;
    listener.setBlocking(false);
    if (listener.listen(DEFAULT_PORT) == sf::Socket::Error) {
        std::cout << "Failed to listen to port " << DEFAULT_PORT << std::endl;
        return;
    }
    TickClock tickClock{60};
    while(true) {
        bool executedTick = tickClock.ExecuteTick([&]() {
            // Recieve Packets
            CheckForDisconnectedClients();
            CheckForNewClient(listener);
            for (auto& [id, client] : clients) {
                client.ProcessPackets([&] (sf::Packet& packet) {
                    PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
                    if (type == PacketFactory::PacketType::PLAYER_INPUT) {
                        PlayerEntity::InputData data = PacketFactory::PlayerInput(packet);
                        PlayerEntity& player = players.at(id);
                        player.Update(tickClock.GetTickDelta(), data);
                        player.Collision(barriers);
                    }
                });
            }
            // Send Movement Packets
            std::vector<PacketFactory::PlayerUpdateData> updateData;
            for (auto& [id, player] : players) {
                std::uint16_t localId = player.id;
#ifdef DEBUG_CLONE
                if(localId == 0) localId = 999;
#endif
                updateData.push_back({ localId, player.getPosition(), player.getRotation() });
            }
            sf::Packet packet{ PacketFactory::PlayerUpdate(updateData) };
            for (auto& [key, value] : clients) {
                value.Send(packet);
            }
        });
        if(!executedTick) {
            sf::sleep(sf::milliseconds(1));
        }
    }
}