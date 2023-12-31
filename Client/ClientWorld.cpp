#include "ClientWorld.h"
#include "ClientPlayerEntity.h"
#include "ClientBarrierEntity.h"
#include "ClientBulletHoleEntity.h"
#include "ClientRocketEntity.h"
#include "ResourceManager.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <format>
#include <algorithm>
#include <iostream>
#include <cassert>

ClientWorld::ClientWorld(std::unique_ptr<ClientNetworking>&& server) : World(World::Side::CLIENT, server->GetServerTick()),
server { std::move(server) },
respawnTime{0.0f},
inputIndex{0},
tickOffset{ 0.1f },
cameraPos{0, 0},
confirmedPlayerStateIndex{ 0 } {
    tickOffset.AddValue(0);
}

void ClientWorld::Update(sf::RenderWindow& window) {
    //World View (for getting the world position of the mouse in update)
    if (auto * player{ GetLocalPlayerEntity() }) {
        cameraPos = player->getPosition();
    }
    window.setView(sf::View{ cameraPos, sf::Vector2f{16, 12} });
    //Tick
    tickClock.ExecuteTick([&]() {
        respawnTime -= tickClock.GetTickDelta();
        if(respawnTime < 0.0f) {
            respawnTime = 0.0f;
        }
        //Receive Packets
        std::optional<PacketFactory::PlayerStateData> confirmedPlayerState;
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
                int tick{ PacketFactory::SetTick(packet) };
                tickOffset.AddValue(tick - tickClock.GetTick());
                server->SendUnreliable(PacketFactory::AckTick(tick));
            }
            if(type == PacketType::TELL_RTT) {
                server->rtt = PacketFactory::TellRTT(packet);
            }
            if(type == PacketType::PLAYER_STATE) {
                PacketFactory::PlayerStateData state{PacketFactory::PlayerState(packet)};
                if(state.index > confirmedPlayerStateIndex) {
                    confirmedPlayerStateIndex = state.index;
                    confirmedPlayerState = state;
                }
            }
        });
        if(auto* player{GetLocalPlayerEntity() }) {
            if (confirmedPlayerState) {
                //Remove unneeded old inputs
                //TODO: given that inputBuffer stores in order of index, this could be more efficient
                std::erase_if(inputBuffer, [=](const PlayerEntity::InputData& data) {
                    return data.index <= confirmedPlayerStateIndex;
                });
                //Rollback and recompute
                player->SetPlayerState(*confirmedPlayerState);
                for (PlayerEntity::InputData& data : inputBuffer) {
                    player->UpdateFromInput(this, data, false);
                }
            }
        }
        //Local Player Update
        if(auto* player{GetLocalPlayerEntity()}) {
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
        // Check TCP connection
        if (tickClock.GetTick() % CHECK_CONNECTION_INTERVAL == 0) {
            server->Send(PacketFactory::None());
        }
    });
}

void ClientWorld::Render(sf::RenderWindow& window) {
    //Render
    if (auto * player{ GetLocalPlayerEntity() }) {
        cameraPos = player->getPosition();
    }
    window.setView(sf::View{ cameraPos, sf::Vector2f{16, 12} });
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
    drawString(std::format("RTT {} ticks", server->rtt), {0, 0});
    if (auto playerOpt = TryGetEntity(localPlayer.value_or(0), EntityType::PLAYER)) {
        ClientPlayerEntity* player{ (ClientPlayerEntity*)playerOpt.value() };
        drawString(std::format("HEALTH {}", player->GetHealth()), {0, 24});
        drawString(std::format("GUN {:.0f}%", 100 - player->GetGunCooldown() * 100), { 0, 48 });
        drawString(std::format("ROCKET {:.0f}%", 100 - player->GetRocketCooldown() * 100), {0, 72});
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

void ClientWorld::FireRocket(EntityID sourceEntity, sf::Vector2f position, float rotation, int lifetime) {
}

bool ClientWorld::TryExplodeRocket(EntityID sourceEntity, EntityID rocketEntity, sf::Vector2f point) {
    return false;
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
    if (type == EntityType::ROCKET) {
        EntityID sourceEntity;
        int creationTick, despawnTick;
        packet >> sourceEntity;
        packet >> creationTick;
        packet >> despawnTick;
        return new ClientRocketEntity(id, position, rotation, sourceEntity, creationTick, despawnTick);
    }
    return nullptr;
}

bool ClientWorld::Disconnected() const {
    return server->GetStatus() == ClientNetworking::Status::DISCONNECTED;
}

std::optional<EntityID> ClientWorld::GetLocalPlayer() const {
    return localPlayer;
}

ClientPlayerEntity* ClientWorld::GetLocalPlayerEntity() const {
    if (!localPlayer) return nullptr;
    std::optional<Entity*> entityOpt{ TryGetEntity(localPlayer.value(), EntityType::PLAYER) };
    if (!entityOpt) return nullptr;
    return (ClientPlayerEntity*)entityOpt.value();
}

int ClientWorld::GetTickOffset() {
    return tickOffset.GetAverage();
}
