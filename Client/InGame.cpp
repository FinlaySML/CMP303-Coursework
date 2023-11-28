#include "InGame.h"
#include <iostream>
#include "ConnectToServer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <array>
#include "BarrierEntity.h"
#include "FontManager.h"
#include <format>

InGame::InGame(std::unique_ptr<sf::TcpSocket>&& server, PacketFactory::JoinGameData data) : 
server{ std::move(server) },
tickClock{ 60 },
localPlayer{ new ClientPlayerEntity(data.playerEntityId, data.position, data.rotation, true) } {
    minigunSoundSource.loadFromFile("guns/minigun.ogg");
    bulletHoleDecal.loadFromFile("bullet_hole.png");
    minigunSound.setBuffer(minigunSoundSource);
    minigunSound.setPitch(0.5f);
    minigunSound.setVolume(30.0f);
    world.AddEntity(localPlayer);
    for(auto& data : data.barriers)  {
        world.AddEntity(new BarrierEntity(data.id, data.position));
    }
}

void InGame::Update(sf::RenderWindow& window) {
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
                    auto entity = world.GetEntity(data.entityId);
                    if (entity.has_value()) {
                        Entity* val{ entity.value() };
                        if (val->GetType() != EntityType::PLAYER) {
                            std::cout << "Client-Server entity type mismatch" << std::endl;
                        } else if (val->GetID() != localPlayer->GetID()) {
                            ClientPlayerEntity* playerEntity{(ClientPlayerEntity*)val};
                            playerEntity->Update(data.position, data.rotation);
                        }
                    } else {
                        world.AddEntity(new ClientPlayerEntity{ data.entityId, data.position, data.rotation });
                    }
                }
            }
            if (type == PacketFactory::PacketType::PLAYER_DAMAGE) {
                auto data = PacketFactory::PlayerDamage(packet);
                auto entity = world.GetEntity(data.id);
                if(entity.has_value()) {
                    Entity* e{entity.value()};
                    if(e->GetType() == EntityType::PLAYER) {
                        ((ClientPlayerEntity*)e)->Damage(data.amount);
                    }else{
                        std::cout << "Can't damage an entity that isn't a player" << std::endl;
                    }
                }else{
                    std::cout << "Can't damage an entity that doesn't exist" << std::endl;
                }
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
        localPlayer->Collision(&world);
        sf::Packet inputPacket{ PacketFactory::PlayerInput(inputData) };
        server.Send(inputPacket);
        if (window.hasFocus()) {
            ClientPlayerEntity::ShootData shootData{localPlayer->UpdateShoot(&world)};
            if(shootData.firedGun) {
                sf::Packet shootPacket{ PacketFactory::PlayerShoot() };
                server.Send(shootPacket);
                minigunSound.play();
                if(shootData.bulletHole) {
                    if(bulletHoles.size() > 100) {
                        bulletHoles.erase(bulletHoles.begin());
                    }
                    bulletHoles.push_back(shootData.bulletHole.value());
                }
            }
        }
    });
}

void InGame::Render(sf::RenderWindow& window) {
    //Render
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    world.Render(window);
    sf::Sprite bhSprite;
    bhSprite.setTexture(bulletHoleDecal);
    bhSprite.setScale(1 / 32.0f, 1 / 32.0f);
    bhSprite.setOrigin(bhSprite.getLocalBounds().getSize()/2.0f);
    for(auto& p : bulletHoles) {
        bhSprite.setPosition(p);
        window.draw(bhSprite);
    }
    //UI
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
    sf::Text text{std::format("HP {}", localPlayer->GetHealth()), FontManager::GetArial()};
    window.draw(text);
}

std::optional<std::unique_ptr<ClientState>> InGame::ChangeState() {
    if(!server.IsConnected()) {
        return std::make_unique<ConnectToServer>();
    }
    return {};
}

sf::Color InGame::GetClearColor() {
    return sf::Color(0xc2a280ff);
}
