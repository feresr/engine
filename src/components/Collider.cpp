#include <Collider.h>
#include "Batch.h"
#include "Input.h"
#include "Ecs.h"
#include <iostream>

bool Collider::renderColliders = false;

Collider::Collider(Engine::RectI &rect)
{
    this->rect = rect;
    kind = Type::Rect;
}

Collider::Collider(int columns, int rows, int tileSize)
{
    grid.columns = columns;
    grid.rows = rows;
    grid.tileSize = tileSize;
    grid.values = new bool[rows * columns]{false};
    kind = Type::Grid;
}

const Collider *Collider::check(uint32_t mask, const glm::ivec2 &offset)
{
    auto colliders = entity->getWorld().componentsOfType<Collider>();

    for (auto *component : colliders)
    {
        auto *collider = (Collider *)component;
        if (this != collider && collider->mask & mask && this->overlaps(*collider, offset))
            return collider;
    }
    return nullptr;
}

bool Collider::overlaps(const Collider &other)
{
    return overlaps(other, glm::vec2(0.0f, 0.0f));
}

bool Collider::overlaps(const Collider &other, const glm::ivec2 &offset)
{
    if (this->kind == Type::Rect && other.kind == Type::Rect)
    {
        const Engine::RectI &a = this->rect + entity->position + offset;
        const Engine::RectI &b = other.rect + other.entity->position;
        return a.overlaps(b);
    }

    if (this->kind == Type::Rect && other.kind == Type::Grid)
    {
        const Engine::RectI &rect = this->rect + entity->position + offset - other.entity->position;
        // check if any of the rect corners fall into a solid tile
        int xstart = glm::clamp((int)glm::floor(rect.left() / (float)other.grid.tileSize), 0, other.grid.columns);
        int xend = glm::clamp((int)glm::ceil(rect.right() / (float)other.grid.tileSize), 0, other.grid.columns);

        int ystart = glm::clamp((int)glm::floor(rect.top() / (float)other.grid.tileSize), 0, other.grid.rows);

        // if bottom = 0 then yend = 0
        int yend = glm::clamp((int)glm::ceil(rect.bottom() / (float)other.grid.tileSize), 0, other.grid.rows);

        for (int i = xstart; i < xend; i++)
        {
            for (int j = ystart; j < yend; j++)
            {
                if (i >= 0 && i < other.grid.columns && j >= 0 && j < other.grid.rows)
                {
                    bool isSolid = other.grid.values[i + j * other.grid.columns];
                    if (isSolid)
                        return true;
                }
            }
        }
    }

    return false;
}

void Collider::setCell(int x, int y, bool value)
{
    ENGINE_ASSERT(kind == Type::Grid, "Incorrect collider type, can't call setCell on a {} type", type);
    grid.values[x + y * grid.columns] = value;
}

bool Collider::getCell(int x, int y)
{
    ENGINE_ASSERT(kind == Type::Grid, "Incorrect collider type, can't call setCell on a {} type", type);
    return grid.values[x + y * grid.columns];
}

void Collider::render(Engine::Batch &batch)
{
    if (renderColliders)
    {
        if (kind == Type::Rect)
        {
            batch.quad(
                entity->position + rect.top_left(),
                rect.size(),
                Engine::Color(255, 0, 0, 160));
        }
        if (kind == Type::Grid)
        {
            for (int c = 0; c < grid.columns; c++)
            {
                for (int r = 0; r < grid.rows; r++)
                {
                    if (grid.values[c + r * grid.columns])
                    {
                        batch.quad(
                            entity->position + glm::ivec2{grid.tileSize * c, grid.tileSize * r},
                            {grid.tileSize, grid.tileSize},
                            Engine::Color(255, 0, 0, 160));
                    }
                }
            }
        }
    }
}

void Collider::update()
{
}

void Collider::clear()
{
    memset(grid.values, false, sizeof(bool) * grid.columns * grid.rows);
}

Collider::~Collider()
{
    delete[] grid.values;
}

void Collider::setRect(Engine::RectI rect)
{
    this->rect = rect;
}
