#pragma once

#include "Component.h"
#include "Collider.h"
#include "Utils.h"

class Kinetic : public Engine::Component {
private:

public:
    glm::vec2 remainder;

    glm::vec2 speed{0.0f, 0.0f};

    float gravity = 0.35f;
    float friction = 0.15;

    std::function<void(Kinetic*)> onHitY = nullptr;
    std::function<void(Kinetic*)> onHitX = nullptr;

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
