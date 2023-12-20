#include "ClientWorld.h"
#include "ClientPlayerEntity.h"
#include "ClientBarrierEntity.h"
#include "ClientBulletHoleEntity.h"
#include "ResourceManager.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <format>
#include <algorithm>
#include <iostream>
#include <cassert>

ClientWorld::ClientWorld(std::unique_ptr<ClientNetworking>&& server) : World(server->GetServerTick()),
server { std::move(server) },
respawnTime{0.0f},
inputIndex{0},
tickOffset{ 0.01f } {
    tickOffset.AddValue(0);
}

void ClientWorld::Update(sf::RenderWindow& window) {
    //World View (for getting the world position of the mouse in update)
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    //Tick
    tickClock.ExecuteTick([&]() {
        respawnTime -= tickClock.GetTickDelta();
        if(respawnTime < 0.0f) {
            respawnTime = 0.0f;
        }
        //Receive Packets
        server->ProcessPackets([&](sf::Packet& packet) {
            PacketType type{ PacketFactory::GetType(packet) };
            if(type == PacketType::STAT_UPDATE) {
                PacketFactory::StatUpdateData data{PacketFactory::StatUpdate(packet)};
                int* stat{server->stats.GetStat(data.type)};
                *stat = data.value;
            }
            if(type == PacketType::MODE_RESPAWNING) {
                respawnTime = PacketFactory::ModeRespawning(packet);
                localPlayer = {};
            }
            if(type == PacketType::ENTITY_CREATE) {
                Entity* entity{ CreateFromPacket(packet)};
                if(entity) {
                    AddEntity(entity);
                }
            }
            if(type == PacketType::ENTITY_UPDATE) {
                EntityID id;
                packet >> id;
                if(id != localPlayer) {
                    if(auto entity = TryGetEntity(id)) {
                        (*entity)->UpdateFromPacket(packet);
                    }
                }
            }
            if(type == PacketType::ENTITY_DELETE) {
                EntityID id{PacketFactory::EntityDelete(packet)};
                RemoveEntity(id);
            }
            if(type == PacketType::MODE_PLAYING) {  
                localPlayer = PacketFactory::ModePlaying(packet);
                respawnTime = 0.0f;
            }
            if (type == PacketType::PLAYER_DAMAGE) {
                auto data{PacketFactory::PlayerDamage(packet)};
                auto entity{GetEntity(data.id, EntityType::PLAYER)};
                ((ClientPlayerEntity*)entity)->Damage(data.amount);
            }
            if (type == PacketType::GUN_EFFECTS) {
                auto data(PacketFactory::GunEffects(packet));
                GunEffects(data.sourceEntity, data.hitEntity, data.hitPosition);
            }
            if (type == PacketType::MESSAGE) {
                std::string message = PacketFactory::Message(packet);
                std::cout << message << std::endl;
            }
            if (type == PacketType::SET_TICK) {
                tickOffset.AddValue(PacketFactory::SetTick(packet) - tickClock.GetTick());
            }
            if(type == PacketType::PLAYER_STATE) {
                if (auto playerOpt = TryGetEntity(localPlayer.value_or(0), EntityType::PLAYER)) {
                    ClientPlayerEntity* player{ (ClientPlayerEntity*)playerOpt.value() };
                    PacketFactory::PlayerStateData state{PacketFactory::PlayerState(packet)};
                    if(state.index > 0) {
                        //Remove unneeded old inputs
                        //TODO: given that inputBuffer stores in order of index, this could be more efficient
                        std::erase_if(inputBuffer, [=](const PlayerEntity::InputData& data) {
                            return data.index <= state.index;
                         });
                        //Rollback and recompute
                        player->SetPlayerState(state);
                        for(PlayerEntity::InputData& data : inputBuffer) {
                            player->UpdateFromInput(this, data, false);
                        }
                    }
                }
            }
        });
        //Local Player Update
        if(auto playerOpt = TryGetEntity(localPlayer.value_or(0), EntityType::PLAYER)) {
            ClientPlayerEntity* player{ (ClientPlayerEntity*)playerOpt.value() };
            //Get input
            inputBuffer.push_back(GetInputData(window));
            //Update with input
            player->UpdateFromInput(this, inputBuffer.back());
            server->SendUnreliable(PacketFactory::PlayerInput(inputBuffer.back()));
        }
        for(auto& [id, entity] : entities) {
            entity->Update(this);
        }
        CleanEntities();
    });
}

void ClientWorld::Render(sf::RenderWindow& window) {
    //Render
    window.setView(sf::View{ sf::Vector2f{0, 0}, sf::Vector2f{16, 12} });
    for (auto& [id, entity] : entities) {
        entity->Draw(window, tickClock.GetTick());
    }
    //UI
    window.setView(sf::View{ sf::Vector2f{window.getSize()} / 2.0f, sf::Vector2f{window.getSize()} });
    auto drawString = [&](const std::string& value, sf::Vector2f pos) {
        sf::Text text{ value, ResourceManager::GetInstance().arial };
        text.setPosition(pos);
        window.draw(text);
    };
    drawString(std::format("PING {} ms", server->GetPing()), {0, 0});
    if (auto playerOpt = TryGetEntity(localPlayer.value_or(0), EntityType::PLAYER)) {
        ClientPlayerEntity* player{ (ClientPlayerEntity*)playerOpt.value() };
        drawString(std::format("HP {}", player->GetHealth()), {0, 24});
    }else if (respawnTime > 0) {
        drawString(std::format("RESPAWN {:.1f}", respawnTime), {0, 24});
    }
    drawString(std::format("KILLS {}", server->stats.kills), { 0, (float)window.getSize().y - 4 * 24 - 8 });
    drawString(std::format("DEATHS {}", server->stats.deaths), { 0, (float)window.getSize().y - 3 * 24 - 8 });
    drawString(std::format("HITS {}", server->stats.hits), { 0, (float)window.getSize().y - 2 * 24 - 8 });
    drawString(std::format("MISSES {}", server->stats.misses), { 0, (float)window.getSize().y - 1 * 24 - 8 });
}

void ClientWorld::GunEffects(EntityID sourceEntity, EntityID hitEntity, sf::Vector2f hitPosition) {
    ResourceManager::GetInstance().minigunSound.play();
}

PlayerEntity::InputData ClientWorld::GetInputData(sf::RenderWindow& window) {
    inputIndex++;
    PlayerEntity::InputData inputData{};
    inputData.index = inputIndex;
    if (window.hasFocus()) {
        inputData.w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        inputData.a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        inputData.s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        inputData.d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        inputData.leftMouse = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        inputData.rightMouse = sf::Mouse::isButtonPressed(sf::Mouse::Right);
        inputData.target = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    } else if(inputBuffer.size() > 0) {
        inputData.target = inputBuffer.back().target;
    }
    return inputData;
}

Entity* ClientWorld::CreateFromPacket(sf::Packet& packet) {
    //Packet type should already be consumed
    EntityType type;
    EntityID id;
    int tick;
    sf::Vector2f position;
    float rotation;
    packet >> (EntityTypeUnderlying&)type;
    packet >> id;
    packet >> tick;
    packet >> position.x >> position.y;
    packet >> rotation;
    if (type == EntityType::PLAYER) {
        return new ClientPlayerEntity(id, tick, position, rotation);
    }
    if (type == EntityType::BARRIER) {
        return new ClientBarrierEntity(id, position);
    }
    if (type == EntityType::BULLET_HOLE) {
        int creationTick, despawnTick;
        sf::Color color;
        packet >> creationTick >> despawnTick;
        packet >> color.r >> color.g >> color.b >> color.a;
        return new ClientBulletHoleEntity(id, position, rotation, creationTick, despawnTick, color);
    }
    return nullptr;
}

bool ClientWorld::Disconnected() const {
    return server->GetStatus() == ClientNetworking::Status::DISCONNECTED;
}

std::optional<EntityID> ClientWorld::GetLocalPlayer() const {
    return localPlayer;
}

int ClientWorld::GetTickOffset() {
    return tickOffset.GetAverage();
}
