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
    entities.remove(entity);
    delete entity;
}

void Engine::World::update()
{
    for (size_t typeIndex = 0; typeIndex < Component::Types::count(); typeIndex++)
    {
        auto &componentArray = components[typeIndex];
        auto component = componentArray.begin();
        while (component != componentArray.end())
        {
            auto next = std::next(component, 1);
            if ((*component)->active && (*component)->entity->alive)
                (*component)->update();
            component = next;
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
    auto iter = entities.begin();
    while (iter != entities.end())
    {
        destroyEntity(*iter++);
        // learning cpp: adding iter++ on this line causes a bug
        // destroy entity removes the entity from the list and invalidates the iterator
        // jumping to the next element fixes the problem
        // since you are now pointing to the next element in the list
    }
}

void Engine::World::destroyComponent(Engine::Component *component)
{
    auto type = component->type;

    std::list<Component *> &all = components[type];
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