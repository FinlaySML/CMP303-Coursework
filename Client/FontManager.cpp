#include "FontManager.h"

const sf::Font& FontManager::GetArial() {
    static sf::Font arial{};
    static bool loaded{false};
    if(!loaded) {
        arial.loadFromFile("arial.ttf");
        loaded = true;
    }
    return arial;
}