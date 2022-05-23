#include "Component.h"

bool Engine::Component::awake()
{
    return true;
}

void Engine::Component::update()
{
}

void Engine::Component::render(Engine::Batch &batch)
{
}

glm::ivec2 &Engine::Component::position()
{
    return entity->position;
};