#pragma once

#include <list>
#include <vector>

#include "glm/glm.hpp"
#include "Log.h"
#include "Component.h"

namespace Engine
{

    class Batch;

    class World;

    class Entity
    {

        friend class World;

    private:
        bool alive = true;

        std::vector<Component *> components;

        Entity(bool alive, glm::vec2 pos, World *world) : alive{alive}, position{pos}, world{world} {}

        World *world;

    public:
        World &getWorld()
        {
            return *world;
        }

        std::vector<Component *> &getComponents();

        glm::ivec2 position;
        float rotation = 0.0f;
        glm::vec2 scale = {1.0f, 1.0f};

        [[nodiscard]] const std::vector<Component *> &getComponents() const;

        template <typename ComponentType, typename... Args>
        ComponentType &add(Args &&...arguments);

        void remove(Component *component);

        template <class T>
        const T *get() const;

        template <class T>
        T *get();

        std::vector<Component *>::iterator begin()
        {
            return components.begin();
        }

        std::vector<Component *>::iterator end()
        {
            return components.end();
        }
    };

    class World
    {

    public:
        World();

        World(const World &) = delete;

        World(World &&) = delete;

        World &operator=(const World &) = delete;

        World &operator=(World &&) = delete;

        ~World();

        Entity *addEntity(glm::vec2 position = glm::vec2{});

        void destroyEntity(Entity *entity);

        void update();

        template <class T>
        void render(Engine::Batch &batch)
        {
            std::list<Component *> alive{};
            auto type = Component::Types::id<T>();
            auto &componentArray = components[type];
            for (auto *component : componentArray)
            {
                if (component->visible && component->entity->alive)
                    alive.push_back(component);
            }
            alive.sort([](const Engine::Component *a, const Engine::Component *b)
                       { return a->depth > b->depth; });
            for (auto *component : alive)
            {
                component->render(batch);
            }
        }

        void clear();

        template <class T>
        T *add(Entity *entity, T *component);

        template <class T>
        T *first() const
        {
            auto type = Component::Types::id<T>();
            return (T *)components[type].front();
        }

        template <typename T>
        [[nodiscard]] typename std::list<Engine::Component *> componentsOfType() const
        {
            auto type = Component::Types::id<T>();
            return components[type];
        }

        std::list<Entity *>::iterator begin()
        {
            return entities.begin();
        }

        std::list<Entity *>::iterator end()
        {
            return entities.end();
        }

    private:
        static constexpr int MAX_COMPONENTS = 256;

        std::list<Entity *> entities{};

        // When adding a component to an entity, the component is stored
        // both in the entity and in 'components[]' for faster iteration* over the same type of component, see: World:add()
        // (*not for faster iteration, this still is a vector of *pointers* which can be scattered around in memory,
        // The real goal of having this `components` vector is to update by component type and not by entity
        // (all transform components, all collider components.. )

        // "World" owns these pointers and is in charge of deallocating them
        // todo: would be nice to make this a vector of Components and not Components* so as to have them packed in
        //  contiguous memory. For now this should do
        std::list<Component *> components[MAX_COMPONENTS];

        void destroyComponent(Component *component);

        friend class Entity;
    };
}

template <typename ComponentType, typename... Args>
ComponentType &Engine::Entity::add(Args &&...arguments)
{
    auto *component = new ComponentType(arguments...);
    return *world->add(this, component);
}

template <class T>
T *Engine::World::add(Entity *entity, T *component)
{
    ENGINE_ASSERT(entity, "Entity cannot be null");
    ENGINE_ASSERT(entity->world == this, "Entity must be part of this world");

    component->entity = entity;
    component->type = Component::Types::id<T>();
    entity->components.emplace_back(component);

    components[component->type].push_back(component);

    component->awake();
    return component;
}

template <class T>
const T *Engine::Entity::get() const
{
    ENGINE_ASSERT(world, "Entity must be assigned to a World");
    auto type = Component::Types::id<T>();
    for (auto *component : components)
    {
        if (component->type == type)
            return (T *)component;
    }
    return nullptr;
}

template <class T>
T *Engine::Entity::get()
{
    ENGINE_ASSERT(world, "Entity must be assigned to a World");
    auto type = Component::Types::id<T>();
    for (auto *component : components)
    {
        if (component->type == type)
            return (T *)component;
    }
    return nullptr;
}