#include "Entity.h"
#include "Entity.hpp"

std::vector<Engine::Component *> &Engine::Entity::getComponents()
{
    return components;
}

const std::vector<Engine::Component *> &Engine::Entity::getComponents() const
{
    return components;
}

void Engine::Entity::remove(Engine::Component *component)
{
    // Remove the component from the entity
    for (auto i = components.begin(); i != components.end(); i++)
    {
        if (*i == component)
        {
            components.erase(i);
            break;
        }
    }
    // Remove it from the world (we keep it both places for easier iteration)
    world->destroyComponent(component);
}