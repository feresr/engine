#pragma once

#include "Component.h"
#include "Collider.h"
#include "Utils.h"

class KineticComponent : public Engine::Component {
private:

public:
    glm::vec2 remainder;

    glm::vec2 speed{0.0f, 0.0f};

    float gravity = 0.35f;
    float friction = 0.15;

    std::function<void(KineticComponent*)> onHitY = nullptr;
    std::function<void(KineticComponent*)> onHitX = nullptr;

    Collider *collider;

    bool moveX(int amount);

    bool moveY(int amount);

    void stopX();

    void stopY();

    void stop();

    bool onGround(int dist = 1) const;

    void update() override;

    void render(Engine::Batch& batch) override;

};
