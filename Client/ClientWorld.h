#pragma once
#include <World.h>
#include "ConnectedSocket.h"
#include "ClientPlayerEntity.h"
#include "PacketFactory.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>

class ClientWorld : public World {
public:
	ClientWorld(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data);
    void Update(sf::RenderWindow& window);
	void Render(sf::RenderWindow& window);
    bool Disconnected() const;
private:
    void GunEffects(const PacketFactory::GunEffectData& data);
    //Network
    ConnectedSocket server;
    //Input
    sf::Vector2f lastMousePosition;
    //Resource
    sf::SoundBuffer minigunSoundSource;
    sf::Sound minigunSound;
    sf::Texture bulletHoleDecal;
    sf::Texture brickTexture;
    //Other
    struct BulletHoleData {
        sf::Vector2f position;
        sf::Color color;
        int creationTick;
        int despawnTime;
    };
    std::vector<BulletHoleData> bulletHoles;
    ClientPlayerEntity* localPlayer;
};

