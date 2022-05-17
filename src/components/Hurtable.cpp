#include "Hurtable.h"
#include "Ecs.h"

void Engine::Hurtable::update() {
    if (hurtBy) {
        if (collider && collider->check(hurtBy)) {
            if (onHurt) onHurt(entity);
        }
    }
}
