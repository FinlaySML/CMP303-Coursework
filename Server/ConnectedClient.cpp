#include "ConnectedClient.h"

ConnectedClient::ConnectedClient(ConnectedSocket&& socket, ClientID id) : socket{std::move(socket)}, id{id}, player{nullptr} {}
