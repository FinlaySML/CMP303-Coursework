#pragma once
#include "ConnectedSocket.h"
#include "Entity.h"
class ServerPlayerEntity;

using ClientID = std::uint16_t;

struct ConnectedClient {
    ConnectedClient(ConnectedSocket&& socket, ClientID id);
    ConnectedSocket socket;
    ClientID id;
    //May be nullptr
    ServerPlayerEntity* player;
};

