#pragma once

#include "Component.h"
#include "rectI.h"
#include "Utils.h"

class CameraComponent : public Engine::Component {

private:

    glm::ivec2 screenSize;
    // 0 = the camera won't move at all
    // 1 = the camera will follow the target perfectly
    // any vaule in between = will slowly approach the target
    glm::vec2 inertia = {1.0f, 1.0f};
    glm::ivec2 offset = {0.0f, 0.0f};
    glm::vec2 reminder = {};

    Engine::Entity* targetEntity = nullptr;
    Engine::RectI bounds{};
    bool fitToBounds = false;

    glm::mat3x2 matrix{ 1.0 };

public:

    explicit CameraComponent(const glm::ivec2& screenSize, const glm::vec2& offset = {});
    explicit CameraComponent(const glm::ivec2& screenSize, const glm::vec2& offset, const glm::vec2& inertia);

    const glm::mat3x2& getMatrix() ;

    void render(Engine::Batch& batch) override;
    void setBounds(const Engine::RectI& bounds);
    void shouldFitToBounds(bool value);

    void follow(Engine::Entity* e);
    void update() override;

    ~CameraComponent();
};