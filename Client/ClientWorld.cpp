#include "ClientWorld.h"
#include "ClientPlayerEntity.h"
#include "BarrierEntity.h"
#include "FontManager.h"
#include <format>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>

ClientWorld::ClientWorld(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : World(data.tick), 
server { std::move(server) },
localPlayer{ new ClientPlayerEntity(data.playerEntityId, data.position, data.rotation, true) } {
    minigunSoundSource.loadFromFile("guns/minigun.ogg");
    bulletHoleDecal.loadFromFile("bullet_hole.png");
    minigunSound.setBuffer(minigunSoundSource);
    minigunSound.setPitch(0.5f);
    minigunSound.setVolume(30.0f);
    AddEntity(localPlayer);
    for (auto& data : data.barriers) {
        AddEntity(new BarrierEntity(data.id, data.position));
    }
    brickTexture.loadFromFile("brick.png");
}

void ClientWorld::Update(sf::RenderWindow& window) {
    //World View (for getting the world position of the mouse in update)
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    //Tick
    tickClock.ExecuteTick([&]() {
        //Receive Packets
        server.ProcessPackets([&](sf::Packet& packet) {
            PacketFactory::PacketType type{ PacketFactory::GetType(packet) };
            if (type == PacketFactory::PacketType::PLAYER_UPDATE) {
                auto updateData = PacketFactory::PlayerUpdate(packet);
                for (const auto& data : updateData) {
                    auto entity = GetEntity(data.entityId);
                    if (entity.has_value()) {
                        Entity* val{ entity.value() };
                        if (val->GetType() != EntityType::PLAYER) {
                            std::cout << "Client-Server entity type mismatch" << std::endl;
                        } else if (val->GetID() != localPlayer->GetID()) {
                            ClientPlayerEntity* playerEntity{ (ClientPlayerEntity*)val };
                            playerEntity->Update(data.position, data.rotation);
                        }
                    } else {
                        AddEntity(new ClientPlayerEntity{ data.entityId, data.position, data.rotation });
                    }
                }
            }
            if (type == PacketFactory::PacketType::PLAYER_DAMAGE) {
                auto data = PacketFactory::PlayerDamage(packet);
                auto entity = GetEntity(data.id);
                if (entity.has_value()) {
                    Entity* e{ entity.value() };
                    if (e->GetType() == EntityType::PLAYER) {
                        ((ClientPlayerEntity*)e)->Damage(data.amount);
                    } else {
                        std::cout << "Can't damage an entity that isn't a player" << std::endl;
                    }
                } else {
                    std::cout << "Can't damage an entity that doesn't exist" << std::endl;
                }
            }
            if (type == PacketFactory::PacketType::GUN_EFFECTS) {
                PacketFactory::GunEffectData data{PacketFactory::GunEffects(packet)};
                GunEffects(data);
            }
            if (type == PacketFactory::PacketType::MESSAGE) {
                std::string message = PacketFactory::Message(packet);
                std::cout << message << std::endl;
            }
        });
        //Local Player Update
        PlayerEntity::InputData inputData;
        inputData.target = lastMousePosition;
        if (window.hasFocus()) {
            inputData = localPlayer->GetInputData(window);
            lastMousePosition = inputData.target;
        }
        localPlayer->UpdateWithInput(tickClock.GetTickDelta(), inputData);
        localPlayer->Collision(this);
        sf::Packet inputPacket{ PacketFactory::PlayerInput(inputData) };
        server.Send(inputPacket);
        if (window.hasFocus()) {
            std::optional<PacketFactory::GunEffectData> shootData{ localPlayer->UpdateShoot(this) };
            if (shootData.has_value()) {
                sf::Packet shootPacket{ PacketFactory::PlayerShoot(tickClock.GetTick()) };
                server.Send(shootPacket);
                GunEffects(shootData.value());
            }
        }
    });
}

void ClientWorld::Render(sf::RenderWindow& window) {
    //Render
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    sf::Sprite brick{ brickTexture };
    brick.setScale(1.0f / 16.0f, 1.0f / 16.0f);
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::BARRIER) {
            brick.setPosition(entity->getPosition());
            window.draw(brick);
        }
    }
    for (auto& [id, entity] : entities) {
        if (entity->GetType() == EntityType::PLAYER) {
            window.draw(*(ClientPlayerEntity*)entity.get());
        }
    }
    std::erase_if(bulletHoles, [&](const BulletHoleData& x) { return (x.creationTick + x.despawnTime) < tickClock.GetTick(); });
    sf::Sprite bhSprite;
    bhSprite.setTexture(bulletHoleDecal);
    bhSprite.setScale(1 / 32.0f, 1 / 32.0f);
    bhSprite.setOrigin(bhSprite.getLocalBounds().getSize() / 2.0f);
    for (auto& p : bulletHoles) {
        bhSprite.setPosition(p.position);
        sf::Color c{p.color};
        c.a *= 1.0f - (tickClock.GetTick() - p.creationTick) / (float)p.despawnTime;
        bhSprite.setColor(c);
        window.draw(bhSprite);
    }
    //UI
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
    sf::Text text{ std::format("HP {}", localPlayer->GetHealth()), FontManager::GetArial() };
    window.draw(text);
}

bool ClientWorld::Disconnected() const {
    return !server.IsConnected();
}

void ClientWorld::GunEffects(const PacketFactory::GunEffectData& data) {
    minigunSound.play();
    if (data.bulletHole) {
        BulletHoleData holeData;
        holeData.position = data.bulletHole.value();
        holeData.color = sf::Color::Black;
        holeData.creationTick = tickClock.GetTick();
        holeData.despawnTime = 1200;
        bulletHoles.push_back(holeData);
    }
}
