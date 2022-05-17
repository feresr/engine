#include "Bird.h"

Bird::Bird() = default;

bool Bird::awake()
{
    auto &s = entity->add<Engine::SpriteComponent>("bird");
    glm::ivec2 size = s.getCurrentAnimSize();
    auto &c = entity->add<ColliderComponent>(Engine::RectI(-size / 2, size));
    c.mask = ColliderComponent::Mask::SOLID;
    auto &k = entity->add<KineticComponent>();
    k.gravity = 0.25;
    k.friction = -0.02;
    k.collider = &c;
    return true;
}
void Bird::update()
{
    auto *sprite = entity->get<Engine::SpriteComponent>();
    auto *kinetic = entity->get<KineticComponent>();

    if (Engine::Input::pressed(Engine::UP) || Engine::Input::pressed(Engine::W))
        kinetic->speed = {0.0, -8.0f};

    kinetic->speed.y >= 0.0 ? sprite->play("U") : sprite->play("D");
    entity->rotation = kinetic->speed.y * 0.1f;
}

void Bird::render(Engine::Batch &batch)
{
}
