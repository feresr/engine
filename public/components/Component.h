#pragma once
#include "Log.h"
#include "glm/glm.hpp"
#include "Entity.h"

namespace Engine
{
    class World;
    class Batch;

    struct Component
    {

        friend class World;

        friend class Entity;

        bool active = true;
        bool visible = true;
        int depth = 0;

        Component()
        {
            ENGINE_CORE_INFO("Component created");
        };

        ~Component()
        {
            ENGINE_CORE_INFO("Component destroyed");
        }

        virtual bool awake();

        virtual void update();

        virtual void render(Engine::Batch &batch);

        glm::ivec2 &position();

        uint8_t type{};

        Entity *getEntity() const
        {
            return entity;
        }

        template <class T>
        T *get()
        {
            return entity->get<T>();
        }

        template <class T, class... Args>
        T& add(Args &&...arguments) {
            return entity->add<T>(arguments...);
        }

        class Types
        {

        public:
            static uint8_t count() { return counter; }

            // The preprocessor will create a template of this function per class (T) that calls it.
            // The only classes that can call it are Id (since it's define in this scope)
            // So each class will have its own ID
            template <class R>
            static uint8_t id()
            {
                static const uint8_t value = Types::counter++;
                return value;
            }

        private:
            static inline uint8_t counter = 0;
        };

    protected:
        Entity *entity = nullptr;
    };
}