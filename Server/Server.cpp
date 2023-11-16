#include <SFML/Network.hpp>
#include <PacketFactory.h>
#include <PlayerEntity.h>
#include <TickClock.h>
#include <iostream>
#include <unordered_map>

std::uint16_t GetNewID(){
    static std::uint16_t usedEntityIds{0};
    usedEntityIds++;
    return usedEntityIds - 1;
}

std::unordered_map<std::uint16_t, std::unique_ptr<sf::TcpSocket>> clients;
std::unordered_map<std::uint16_t, PlayerEntity> players;

void CheckForNewClient(sf::TcpListener& listener) {
    std::unique_ptr<sf::TcpSocket> socket{std::make_unique<sf::TcpSocket>()};
    socket->setBlocking(false);
    if (listener.accept(*socket) != sf::TcpListener::Done) {
        return;
    }
    std::uint16_t id{ GetNewID() };
    sf::TcpSocket* client = clients.insert({ id, std::move(socket) }).first->second.get();
    PlayerEntity& newPlayer = players.insert({ id, PlayerEntity{id, {0.5f, 0.5f}, 0.0f} }).first->second;
    sf::Packet packet{ PacketFactory::JoinGame(newPlayer.id, newPlayer.getPosition(), newPlayer.getRotation()) };
    client->send(packet);
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
                for (sf::Packet packet; client->receive(packet) == sf::TcpSocket::Done; packet.clear()) {
                    PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
                    if (type == PacketFactory::PacketType::PLAYER_INPUT) {
                        PlayerEntity::InputData data = PacketFactory::PlayerInput(packet);
                        players.at(id).Update(tickClock.GetTickDelta(), data);
                    }
                }
            }
            // Send Packets
            std::vector<PacketFactory::PlayerUpdateData> updateData;
            for (auto& [id, player] : players) {
                updateData.push_back({ player.id, player.getPosition(), player.getRotation() });
            }
            sf::Packet packet{ PacketFactory::PlayerUpdate(updateData) };
            for (auto& [id, client] : clients) {
                client->send(packet);
            }
        });
        if(!executedTick) {
            sf::sleep(sf::milliseconds(1));
        }
    }
}