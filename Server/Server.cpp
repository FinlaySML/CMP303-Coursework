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
    ServerWorld world{ DEFAULT_PORT };
    if(!world.Listening()) {
        return -1;
    }
    while(true) {
        world.Update();
    }
    return 0;
}