#include "Floor.h"
#include "Slider.h"


Floor::Floor(int width, int height) : width{width}, height{height} {

}

bool Floor::awake()
{
    entity->add<Slider>();
    auto base_texture = base->getAnimation()->frames[0].texture;
    auto &c = entity->add<Collider>(Engine::RectI(
        0.0, height - base_texture.height(), width * 2, base_texture.height()));
    c.mask = Collider::Mask::SOLID;
    return true;
}

void Floor::update()
{
    if (entity->position.x <= -width)
        entity->position.x = 0;
}

void Floor::render(Engine::Batch &batch)
{
    auto base_texture = base->getAnimation()->frames[0].texture;
    batch.tex(base_texture, {entity->position.x, height - base_texture.height()}, 0xffffff);
    batch.tex(base_texture, {entity->position.x + width, height - base_texture.height()}, 0xffffff);
}