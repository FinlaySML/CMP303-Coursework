#pragma once
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class ResourceManager {
    static ResourceManager* instance;
public:
    static ResourceManager& GetInstance();
    ResourceManager();
    //Resource
    sf::SoundBuffer minigunSoundSource;
    sf::Sound minigunSound;
    sf::Texture bulletHoleDecal;
    sf::Texture brickTexture;
    sf::Font arial;
};

