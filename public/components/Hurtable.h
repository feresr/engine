#pragma once

#include "Component.h"
#include "ColliderComponent.h"

namespace Engine {
    class Hurtable : public Component {
    public:

        void update() override;

        uint32_t hurtBy = ColliderComponent::Mask::NONE;

        std::function<void(Entity * )> onHurt = nullptr;

        ColliderComponent *collider = nullptr;
    private:


    };
}