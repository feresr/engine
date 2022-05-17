#include "Background.h"

Background::Background(int width, int height) : width{width}, height{height}
{
}

bool Background::awake()
{
    auto base_texture = base->getAnimation()->frames[0].texture;
    auto& c = entity->add<ColliderComponent>(Engine::RectI(
        0.0, height - base_texture.height(), width * 2, base_texture.height()
    ));
    c.mask = ColliderComponent::Mask::SOLID;
    return true;
}

void Background::update()
{
    this->entity->position.x -= 1;
    if (this->entity->position.x <= -width)
        this->entity->position.x = 0;
}

void Background::render(Engine::Batch &batch)
{
    auto sky_texture = sky->getAnimation()->frames[0].texture;
    auto base_texture = base->getAnimation()->frames[0].texture;
    batch.tex(sky_texture, entity->position, 0xffffff);
    batch.tex(sky_texture, entity->position + glm::ivec2{width, 0}, 0xffffff);
    batch.tex(base_texture, {entity->position.x, height - base_texture.height()}, 0xffffff);
    batch.tex(base_texture, {entity->position.x + width, height - base_texture.height()}, 0xffffff);
}
