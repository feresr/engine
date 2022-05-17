#pragma once

#include "Component.h"

namespace Engine {

    class TimerComponent : public Component {

        void update() override;

        bool repeats = false;

        float internalTimer = 0;
    public:

        TimerComponent(float timer, bool repeats = false);

        float timer = 0;
        // todo: bool means "continue execution?", when deleing the entity or component this
        // lambda should return 'false' to avoid continuing executing code on a dead 'TimerComponent' object- 
        // 'true' otherwise.
        std::function<bool(Entity*)> timerAction = nullptr;

    };
}