#include "Bird.h"

Bird::Bird() = default;

bool Bird::awake()
{
    auto &s = add<Engine::SpriteComponent>("bird");
    glm::ivec2 size = s.getCurrentAnimSize();
    auto rect = Engine::RectI(-size / 2, size);
    rect.x += 6;
    rect.y += 6;
    rect.w -= 12;
    rect.h -= 6;
    add<Collider>(rect);
    get<Collider>()->mask = Collider::Mask::SOLID;

    add<Kinetic>();
    get<Kinetic>()->gravity = 0.25;
    get<Kinetic>()->collider = get<Collider>();
    return true;
}
void Bird::update()
{
    // helpers
    auto *sprite = get<Engine::SpriteComponent>();
    auto *kinetic = get<Kinetic>();
    auto *collider = get<Collider>();

    // flap
    if (!dead && (Engine::Input::pressed(Engine::UP) || Engine::Input::pressed(Engine::W)))
        kinetic->speed = {0.0, -6.0f};
    kinetic->speed.y >= 0.0 ? sprite->play("U") : sprite->play("D");
    sprite->rotation = std::min(kinetic->speed.y * (dead ? 0.2f : 0.1f), 3.1415f / 2.0f);

    // face down when dead in the floor
    if (kinetic->speed.y == 0 && dead)
        sprite->rotation = 3.1415f / 2.0f;

    // dead?
    if (!dead && collider->check(Collider::Mask::ENEMY))
    {
        onPipeCrash();
        kinetic->speed = {0.0, 1.0f};
    }
}

void Bird::render(Engine::Batch &batch)
{
    get<Engine::SpriteComponent>()->render(batch);
}
