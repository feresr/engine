#include "Pipe.h"
#include "Slider.h"

Pipe::Pipe(bool top) : top{top} {}

bool Pipe::awake()
{
    add<Slider>();
    auto &s = add<Engine::SpriteComponent>("pipe");
    auto size = s.getCurrentAnimSize();
    auto &c = add<Collider>(Engine::RectI(-size.x / 2, 0, size.x, size.y));
    c.mask = Collider::Mask::ENEMY;
    if (top)
    {
        s.scale.y = -1.00;
        c.setRect(Engine::RectI(-size.x / 2, -size.y, size.x, size.y));
    }
    return true;
}

void Pipe::update()
{
}

void Pipe::render(Engine::Batch &batch)
{
    auto spriteSize = get<Engine::SpriteComponent>()->getCurrentAnimSize();
    batch.pushMatrix(Engine::Math::transform({0.0f, 0.0f}, {spriteSize.x / 2.0f, 0.0}, {1.0, 1.0}));
    entity->get<Engine::SpriteComponent>()->render(batch);
    batch.popMatrix();
}