#pragma once
#include "Entity.h"
#include "Component.h"
#include "Ecs.h"

namespace Engine
{
    class Entity;

    template <class T>
    T *Entity::get()
    {
        ENGINE_ASSERT(world, "Entity must be assigned to a World");
        auto type = Component::Types::id<T>();
        for (auto *component : components) {
            if (component->type == type)
                return (T *)component;
        }
        return nullptr;
    }

    template <typename ComponentType, typename... Args>
    ComponentType &Entity::add(Args &&...arguments)
    {
        auto *component = new ComponentType(arguments...);
        return *world->add(this, component);
    }
};