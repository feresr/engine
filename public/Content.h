#pragma once

#include <memory>
#include <sstream>
#include "rectI.h"
#include <map>

namespace Engine
{
    class Sprite;
};

struct MapInfo
{
    std::string fileName;
    Engine::RectI rect;
};

class Content
{

public:
    static void load();

    static std::string path();
    static void playMusic();
    static void playSound();
    static void playSoundHit();
    static void playSoundJump();

    static std::vector<MapInfo> getMaps();

    static Engine::Sprite *findSprite(const std::string &name);

    static MapInfo *findMapInfo(const glm::ivec2 &position);

private:
    static std::map<std::string, Engine::Sprite> sprites;
    static std::vector<MapInfo> maps;

    Content() = default;
};
