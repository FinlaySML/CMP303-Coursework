#include "ServerNetworking.h"
#include "PacketFactory.h"
#include <iostream>
#include <limits>
#include "ServerPlayerEntity.h"
#include <cassert>

ServerNetworking::ServerNetworking(unsigned short port) : usedClientIds{0} {
    listener.setBlocking(false);
    udp.setBlocking(false);
    listener.listen(port);
    udp.bind(port);
}

ClientID ServerNetworking::GetNewClientID() {
    usedClientIds++;
    return usedClientIds;
}

ConnectedClient* ServerNetworking::GetNewClient() {
    auto cc{ std::make_unique<ConnectedClient>(listener, &udp)};
    if(cc->GetStatus() == ConnectedClient::Status::LOADING){
        cc->id = GetNewClientID();
        auto result = clients.insert({cc->id, std::move(cc)});
        return result.first->second.get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<ConnectedClient>> ServerNetworking::GetDisconnectedClients() {
    std::vector<std::unique_ptr<ConnectedClient>> result;
    //Move disconnected clients to result
    for(auto& [id, client] : clients) {
        if(client->GetStatus() == ConnectedClient::Status::DISCONNECTED) {
            result.push_back(std::move(client));
        }
    }
    //Erase entries in 'clients' which became nullptr
    std::erase_if(clients, [](const auto& pair) {
        auto const& [id, client] = pair;
        return client.get() == nullptr;
    });
    return result;
}

void ServerNetworking::ProcessPackets(std::function<void(sf::Packet&, ConnectedClient*)> func) {
    for(auto& [id, client] : clients) {
        for (sf::Packet packet; client->tcp.receive(packet) == sf::Socket::Done; packet.clear()) {
            func(packet, client.get());
        }
    }
    sf::IpAddress clientIp;
    unsigned short clientPort;
    for (sf::Packet packet; udp.receive(packet, clientIp, clientPort) == sf::Socket::Done; packet.clear()) {
        for (auto& [id, client] : clients) {
            if(client->tcp.getRemoteAddress() == clientIp && client->tcp.getRemotePort() == clientPort) {
                func(packet, client.get());
                break;
            }
        }
    }
}

void ServerNetworking::Broadcast(sf::Packet packet, ClientID excluding, bool reliable) {
    for (auto& [id, client] : clients) {
        if (excluding == client->id) continue;
        client->Send(packet, reliable);
    }
}

void ServerNetworking::Broadcast(const std::string& message) {
    sf::Packet joinMessage{ PacketFactory::Message(message) };
    Broadcast(joinMessage);
    std::cout << message << std::endl;
}

std::vector<ConnectedClient*> ServerNetworking::UpdateRespawnTimer(float dt) {
    std::vector<ConnectedClient*> due;
    for(auto& [id, client] : clients) {
        if(client->GetStatus() == ConnectedClient::Status::RESPAWNING){
            client->respawnTime -= dt;
            if(client->respawnTime < 0.0f){
                client->respawnTime = 0.0f;
                due.push_back(client.get());
            }
        }
    }
    return due;
}
