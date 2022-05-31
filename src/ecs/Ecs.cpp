//
// Created by Fernando Raviola on 10/03/2021.
//

#include "Ecs.h"
#include "Batch.h"

// WORLD
Engine::Entity *Engine::World::addEntity(glm::vec2 position)
{
    auto *entity = new Entity{true, position, this};
    entities.push_back(entity);
    return entity;
}

void Engine::World::destroyEntity(Engine::Entity *entity)
{
    ENGINE_ASSERT(entity->world == this, "Entity does not belong to this world")

    auto &components = entity->getComponents();
    auto component = components.begin();
    while (component != components.end())
    {
        auto next = std::next(component, 1);
        destroyComponent(*component);
        component = next;
    }
    entity->world = nullptr;
    entity->alive = false;

    for (auto e = entities.begin(); e != entities.end(); e++)
    {
        if (*e == entity)
        {
            entities.erase(e);
            break;
        }
    }
    delete entity;
}

void Engine::World::update()
{
    for (size_t typeIndex = 0; typeIndex < Component::Types::count(); typeIndex++)
    {
        for (int i = components[typeIndex].size() - 1; i >= 0; i--)
        {
            auto *component = components[typeIndex][i];
            if (component->active && component->entity->alive)
                component->update();
        }
    }
}

Engine::World::World()
{
    ENGINE_CORE_INFO("World()");
}

Engine::World::~World()
{
    clear();
    ENGINE_CORE_INFO("~World()");
}

void Engine::World::clear()
{
    for (int i = entities.size() - 1; i >= 0; i--)
        destroyEntity(entities[i]);
}

void Engine::World::destroyComponent(Engine::Component *component)
{
    auto type = component->type;

    std::vector<Component *> &all = components[type];
    for (auto i = all.begin(); i != all.end(); i++)
    {
        if (*i == component)
        {
            all.erase(i);
            break;
        }
    }

    delete component;
}

// COMPONENT