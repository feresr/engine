#include "Slider.h"

bool Slider::awake()
{
    return true;
}

void Slider::update()
{
    entity->position.x -= velocity;
}

void Slider::render(Engine::Batch &batch)
{
}
