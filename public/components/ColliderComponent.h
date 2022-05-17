#pragma once

#include <rectI.h>
#include "Component.h"

class ColliderComponent : public Engine::Component {

public:

    struct Mask {
        static constexpr uint32_t NONE = 1 << 0;
        static constexpr uint32_t SOLID = 1 << 1;
        static constexpr uint32_t PLAYER_ATTACK = 1 << 2;
        static constexpr uint32_t ENEMY = 1 << 3;
    };

    explicit ColliderComponent(Engine::RectI &rect);

    explicit ColliderComponent(int width, int height, int tileSize);

    bool overlaps(const ColliderComponent &other);

    bool overlaps(const ColliderComponent &other, const glm::ivec2 &offset);

    void setCell(int x, int y, bool value);

    void setRect(Engine::RectI rect);

    void clear();

    bool getCell(int x, int y);

    void render(Engine::Batch &batch) override;

    /*
     * Returns the first collider in the world colliding with [this], nullptr otherwise
     */
    const ColliderComponent *check(uint32_t mask, const glm::ivec2 &offset = { 0.0, 0.0});

    void update() override;

    ~ColliderComponent();

    uint32_t mask = Mask::NONE;

    static bool renderColliders;

private:
    enum class Type {
        None, Grid, Rect
    };
    struct Grid {
        int columns;
        int rows;
        int tileSize;
        bool *values = nullptr;
    };

    Engine::RectI rect;
    Grid grid;
    Type kind = Type::None;

};