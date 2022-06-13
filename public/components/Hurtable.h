#pragma once

#include "Component.h"
#include "Collider.h"

namespace Engine {
    class Hurtable : public Component {
    public:

        void update() override;

        uint32_t hurtBy = Collider::Mask::NONE;

        std::function<void(Entity * )> onHurt = nullptr;

        Collider *collider = nullptr;

    };
}
