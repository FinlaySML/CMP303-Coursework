#include <SFML/Network.hpp>
#include <PacketFactory.h>
#include <PlayerEntity.h>
#include <TickClock.h>
#include <iostream>
#include <unordered_map>
#include <format>
#include "ServerWorld.h"
#include "BarrierEntity.h"

int main()
{
    ServerWorld world{ DEFAULT_PORT };
    while(true) {
        world.Update();
    }
    return 0;
}