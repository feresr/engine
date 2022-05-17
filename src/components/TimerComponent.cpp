#include "TimerComponent.h"
#include "Ecs.h"

void Engine::TimerComponent::update() {
    if (internalTimer > 0) {
        internalTimer -= 1000.0f / 60;
        if (internalTimer <= 0) {
            if (timerAction) { 
                // when timerAction retuns false, it means this component was deleted
                // so we abort execution.
                if (!timerAction(entity)) return;
            }
            if (repeats) { 
                // reset the timer if needed.
                internalTimer = timer;
            } else {
                // remove component (does not repeat, no longer required)
                entity->remove(this);
            }
        }
    }
}

Engine::TimerComponent::TimerComponent(float timer, bool repeats) : timer{timer}, repeats{repeats} {
    internalTimer = timer;
}
