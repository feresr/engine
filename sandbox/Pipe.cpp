#include "Pipe.h"
#include "Slider.h"

Pipe::Pipe(bool top) : top{top} {}

bool Pipe::awake()
{
    entity->add<Slider>();
    auto &s = entity->add<Engine::SpriteComponent>("pipe");
    auto size = s.getCurrentAnimSize();
    auto &c = entity->add<ColliderComponent>(Engine::RectI(-size.x / 2, 0, size.x, size.y));
    c.mask = ColliderComponent::Mask::ENEMY;
    if (top)
    {
        s.scale.y = -1;
        c.setRect(Engine::RectI(-size.x / 2, -size.y, size.x, size.y));
    }
    return true;
}

void Pipe::update()
{
}

void Pipe::render(Engine::Batch &batch)
{
    auto sprite = entity->get<Engine::SpriteComponent>();
    auto spriteSize = sprite->getCurrentAnimSize();
    batch.pushMatrix(Engine::Math::transform({0.0f, 0.0f}, {spriteSize.x / 2.0f, 0.0}, {1.0, 1.0}));
    entity->get<Engine::SpriteComponent>()->render(batch);
    batch.popMatrix();
}