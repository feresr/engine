#pragma once

#include <memory>
#include "Component.h"
#include "Entity.hpp"
#include "Collider.h"
#include "Content.h"

namespace Engine {
    class Subtexture;
}

struct MapObject {
    int x, y;
    std::string type;

    bool operator<(const MapObject& other) const {
        return this->type < other.type;
    }
};

class TileMapComponent : public Engine::Component {

public:
    TileMapComponent(const MapInfo* mapInfo);

    bool awake() override;

    void render(Engine::Batch &batch) override;

    Engine::RectI bounds() const;

    const std::vector<MapObject> getMapObjects() {
        return objects;
    }

private:

    // Player position (used to pick the right tilemap)
    const MapInfo* mapInfo;

    void setCell(unsigned int x, unsigned int y, const Engine::Subtexture &sprite);

    std::vector<Engine::Subtexture> grid{};
    std::vector<MapObject> objects{};

    int rows{}, columns{};

};