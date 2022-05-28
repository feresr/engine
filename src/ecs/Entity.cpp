#include "Entity.h"
#include "Entity.hpp"

void Engine::Entity::destroy() {
    // todo: maybe this should be "marked for destroy"
    // and get removed only after the world is done updating / rendering
    world->destroyEntity(this);
}

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