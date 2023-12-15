#include "ResourceManager.h"

ResourceManager* ResourceManager::instance{nullptr};

ResourceManager& ResourceManager::GetInstance() {
    if(!instance){
        instance = new ResourceManager();
    }
    return *instance;
}

ResourceManager::ResourceManager() {
    minigunSoundSource.loadFromFile("guns/minigun.ogg");
    bulletHoleDecal.loadFromFile("bullet_hole.png");
    minigunSound.setBuffer(minigunSoundSource);
    minigunSound.setPitch(0.5f);
    minigunSound.setVolume(5.0f);
    brickTexture.loadFromFile("brick.png");
    arial.loadFromFile("arial.ttf");
}