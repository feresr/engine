#pragma once

#include <rectI.h>
#include "Collider.h"
#include "Component.h"

class Collider : public Engine::Component {

public:

    struct Mask {
        static constexpr uint32_t NONE = 1 << 0;
        static constexpr uint32_t SOLID = 1 << 1;
        static constexpr uint32_t PLAYER_ATTACK = 1 << 2;
        static constexpr uint32_t COLLECTIBLE = 1 << 2;
        static constexpr uint32_t ENEMY = 1 << 3;
    };

    explicit Collider(Engine::RectI &rect);

    explicit Collider(int columns, int rows, int tileSize);

    bool overlaps(const Collider &other);

    bool overlaps(const Collider &other, const glm::ivec2 &offset);

    void setCell(int x, int y, bool value);

    void setRect(Engine::RectI rect);

    void clear();

    bool getCell(int x, int y);

    void render(Engine::Batch &batch) override;

    /*
     * Returns the first collider in the world colliding with [this], nullptr otherwise
     */
    const Collider *check(uint32_t mask, const glm::ivec2 &offset = { 0.0, 0.0});

    void update() override;

    ~Collider();

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
