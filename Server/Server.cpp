#include <SFML/Network.hpp>
#include <PacketFactory.h>
#include <PlayerEntity.h>
#include <TickClock.h>
#include <ConnectedSocket.h>
#include <iostream>
#include <unordered_map>
#include <format>

std::uint16_t GetNewID(){
    static std::uint16_t usedEntityIds{0};
    usedEntityIds++;
    return usedEntityIds - 1;
}

std::unordered_map<std::uint16_t, ConnectedSocket> clients;
std::unordered_map<std::uint16_t, PlayerEntity> players;

void ServerMessage(const std::string& message) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    ConnectedSocket::SendToAll(clients, joinMessage);
    std::cout << message << std::endl;
}

void CheckForNewClient(sf::TcpListener& listener) {
    ConnectedSocket socket{listener};
    if(!socket.IsConnected()) {
        return;
    }
    std::uint16_t id{ GetNewID() };
    ConnectedSocket& client = clients.insert({ id, std::move(socket) }).first->second;
    PlayerEntity& newPlayer = players.insert({ id, PlayerEntity{id, {0.5f, 0.5f}, 0.0f} }).first->second;
    sf::Packet packet{ PacketFactory::JoinGame(newPlayer.id, newPlayer.getPosition(), newPlayer.getRotation()) };
    client.Send(packet);
    ServerMessage(std::format("A new player ({}) has joined the game", id));
}

void main()
{
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
            CheckForNewClient(listener);
            for (auto& [id, client] : clients) {
                client.ProcessPackets([&] (sf::Packet& packet) {
                    PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
                    if (type == PacketFactory::PacketType::PLAYER_INPUT) {
                        PlayerEntity::InputData data = PacketFactory::PlayerInput(packet);
                        players.at(id).Update(tickClock.GetTickDelta(), data);
                    }
                });
            }
            // Send Movement Packets
            std::vector<PacketFactory::PlayerUpdateData> updateData;
            for (auto& [id, player] : players) {
                updateData.push_back({ player.id, player.getPosition(), player.getRotation() });
            }
            sf::Packet packet{ PacketFactory::PlayerUpdate(updateData) };
            ConnectedSocket::SendToAll(clients, packet);
        });
        if(!executedTick) {
            sf::sleep(sf::milliseconds(1));
        }
    }
}