#pragma once
#include <World.h>
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>
#include "ClientNetworking.h"
#include "MovingAverage.h";

class ClientWorld : public World {
public:
	ClientWorld(std::unique_ptr<ClientNetworking>&& server);
    void Update(sf::RenderWindow& window);
    void Render(sf::RenderWindow& window);
    void GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) override;
    void FireRocket(EntityID sourceEntity, sf::Vector2f position, float rotation, int lifetime) override;
    PlayerEntity::InputData GetInputData(sf::RenderWindow& window);
    static Entity* CreateFromPacket(sf::Packet& packet);
    bool Disconnected() const;
    std::optional<EntityID> GetLocalPlayer() const;
    ClientPlayerEntity* GetLocalPlayerEntity() const;
    int GetTickOffset();
private:
    int inputIndex;
    MovingAverage tickOffset;
    std::vector<PlayerEntity::InputData> inputBuffer;
    std::unique_ptr<ClientNetworking> server;
    std::optional<EntityID> localPlayer;
    float respawnTime;
};

