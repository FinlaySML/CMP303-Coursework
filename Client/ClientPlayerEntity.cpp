#include "ClientPlayerEntity.h"
#include "ClientWorld.h"
#include <iostream>
#include <random>
#include "BulletHoleEntity.h"
#include "ResourceManager.h"

ClientPlayerEntity::ClientPlayerEntity(EntityID entityId, int tick, sf::Vector2f position, float rotation) : 
PlayerEntity(entityId, position, rotation), 
body{0.4f}, 
gun{sf::Vector2f{0.3f, 0.2f}}, 
localPlayer{false}, 
damageReddening{0},
interpolator{tick, 60, position, rotation} {
	body.setFillColor({ 0,128,0 });
	body.setOrigin(body.getRadius(), body.getRadius());
	gun.setFillColor({ 0,64,0 });
	gun.setOrigin(gun.getSize() * 0.5f);
	gun.setPosition(body.getRadius(), 0);
}

void ClientPlayerEntity::Draw(sf::RenderWindow& window, int tick) const {
	window.draw(body, getTransform());
	window.draw(gun, getTransform());
}

void ClientPlayerEntity::UpdateFromPacket(sf::Packet& packet) {
	int tick;
	float x, y, r;
	packet >> tick;
	packet >> x >> y;
	packet >> r;
	interpolator.AddKeyframe(tick, {x, y}, r);
}

void ClientPlayerEntity::Update(World* world) {
	ClientWorld* cWorld{ (ClientWorld*)world };
	localPlayer = cWorld->GetLocalPlayer() == GetID();
	if(!localPlayer) {
		Interpolator::Keyframe kf{interpolator.GetKeyframe(cWorld->GetClock().GetTick()+cWorld->GetTickOffset())};
		setPosition(kf.position);
		setRotation(kf.rotation);
	}
	if (damageReddening > 0) {
		damageReddening--;
		body.setFillColor({ 255,55,0 });
	} else {
		body.setFillColor({ 0,128,0 });
	}
	if (localPlayer) {
		body.setOutlineThickness(0.05f);
		body.setRadius(0.35f);
		body.setOrigin(body.getRadius(), body.getRadius());
	} else {
		body.setOutlineThickness(0.0f);
		body.setRadius(0.4f);
		body.setOrigin(body.getRadius(), body.getRadius());
	}
}

void ClientPlayerEntity::SetPlayerState(PacketFactory::PlayerStateData state) {
	setPosition(state.position);
	setRotation(state.rotation);
	gunCooldown = state.gunCooldown;
	rocketCooldown = state.rocketCooldown;
}

std::mt19937 randGen;
std::uniform_real_distribution<float> bulletSpread{-2, 2};

void ClientPlayerEntity::Damage(int amount) {
	PlayerEntity::Damage(0, amount);
	damageReddening = 4;
}

void ClientPlayerEntity::SetLocalPlayer(bool value) {
	localPlayer = value;
}
