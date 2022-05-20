#include "Background.h"
#include "Slider.h"

Background::Background(int width, int height) : width{width}, height{height}
{
}

bool Background::awake()
{
    this->entity->add<Slider>();
    return true;
}

void Background::update()
{
    if (this->entity->position.x <= -width)
        this->entity->position.x = 0;
}

void Background::render(Engine::Batch &batch)
{
    auto sky_texture = sky->getAnimation()->frames[0].texture;
    batch.tex(sky_texture, entity->position, 0xffffff);
    batch.tex(sky_texture, entity->position + glm::ivec2{width, 0}, 0xffffff);
}
