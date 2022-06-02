#pragma once
#include "Entity.h"
#include <vector>
#include "glm/glm.hpp"

namespace Engine
{
    class World;
    class Component;

    class Entity
    {

    private:
        bool alive = true;

        std::vector<Component *> components;

        Entity(bool alive, glm::vec2 pos, World *world) : alive{alive}, position{pos}, world{world} {}

        static Entity* create(bool alive, glm::vec2 pos, World* world) {
            return new Entity(alive, pos, world);
        }

        World *world;

    public:
        friend class World;
        World &getWorld()
        {
            return *world;
        }

        void destroy();

        std::vector<Component *> &getComponents();

        glm::ivec2 position;

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

}