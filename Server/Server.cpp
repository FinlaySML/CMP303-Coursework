#include <SFML/Network.hpp>
#include <PacketFactory.h>
#include <PlayerEntity.h>
#include <TickClock.h>
#include <ConnectedSocket.h>
#include <iostream>
#include <unordered_map>
#include <format>
#include "ServerWorld.h"
#include "BarrierEntity.h"

int main()
{
    ServerWorld world;
    for (int i = 0; i < 16; i++) {
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( i - 8,-6 )));
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( i - 8, 5 )));
    }
    for (int i = 0; i < 10; i++) {
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( -8,i - 5 )));
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( 7, i - 5 )));
    }
    for (int i = 0; i < 6; i++) {
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( -5,i - 3 )));
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( 4, i - 3 )));
    }
    for (int i = 0; i < 6; i++) {
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( -i + 1,2 )));
        world.AddEntity(new BarrierEntity(world.GetNewID(), sf::Vector2f( i - 2, -3 )));
    }

    sf::TcpListener listener;
    listener.setBlocking(false);
    if (listener.listen(DEFAULT_PORT) == sf::Socket::Error) {
        std::cout << "Failed to listen to port " << DEFAULT_PORT << std::endl;
        return -1;
    }
    TickClock tickClock{60};
    while(true) {
        bool executedTick = tickClock.ExecuteTick([&]() {
            world.DisconnectClients();
            world.ConnectClients(listener);
            world.Tick(tickClock.GetTickDelta());
        });
        if(!executedTick) {
            sf::sleep(sf::milliseconds(1));
        }
    }
    return 0;
}