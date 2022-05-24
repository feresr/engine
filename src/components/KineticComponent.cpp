#include "components/KineticComponent.h"
#include "Ecs.h"

bool KineticComponent::moveX(int amount) {
    if (amount == 0) return false;
    if (collider) {
        glm::ivec2 offset = glm::vec2{amount, 0};
        auto *other = collider->check(Collider::Mask::SOLID, offset);
        if (other) {
            int a = amount;
            int sign = glm::sign(amount);
            while (a != 0) {
                offset.x = a;
                if (!collider->overlaps(*other, offset)) break;
                a -= sign;
            }
            entity->position.x += a;
            if (onHitX) {
                onHitX(this);
            } else {
                stopX();
            }
        } else {
            entity->position.x += amount;
        }
    } else {
        entity->position.x += amount;
    }

    return false;
}

bool KineticComponent::moveY(int amount) {
    if (amount == 0) return false;
    if (collider) {
        glm::ivec2 offset = glm::ivec2{0, amount};
        auto *other = collider->check(Collider::Mask::SOLID, offset);
        if (other) {
            int a = amount;
            int sign = glm::sign(amount);
            while (a != 0) {
                offset.y = a;
                if (!collider->overlaps(*other, offset)) break;
                a -= sign;
            }
            entity->position.y += a;
            if (onHitY) {
                onHitY(this);
            } else {
                stopY();
            }
        } else {
            entity->position.y += amount;
        }
    } else {
        entity->position.y += amount;
    }

    return false;
}

void KineticComponent::stopX() {
    speed.x = 0;
    remainder.x = 0;
}

void KineticComponent::stopY() {
    speed.y = 0;
    remainder.y = 0;
}

void KineticComponent::stop() {
    stopX();
    stopY();
}

bool KineticComponent::onGround(int dist) const {
    if (collider) {
        return collider->check(Collider::Mask::SOLID, glm::ivec2{ 0, dist});
    }
    return false;
}


void KineticComponent::render(Engine::Batch &batch) {
}

void KineticComponent::update() {
    // apply friction
    if (!onGround()) {
        speed.y += gravity;
    }

    auto total = remainder + speed;
    auto move = glm::ivec2((int) total.x, (int) total.y);

    remainder.x = total.x - (int) move.x;
    remainder.y = total.y - (int) move.y;

    moveX(move.x);
    moveY(move.y);
}
