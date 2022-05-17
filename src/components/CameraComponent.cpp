#include <CameraComponent.h>

#include "Batch.h"
#include "Ecs.h"

void CameraComponent::update()
{
    if (targetEntity)
    {
        auto deltaX = ((targetEntity->position.x) - (reminder.x + entity->position.x) - offset.x) * inertia.x;
        auto deltaY = ((targetEntity->position.y) - (reminder.y + entity->position.y) - offset.y) * inertia.y;

        reminder = {deltaX - (int)deltaX, deltaY - (int)deltaY};

        entity->position.x += (int)deltaX;
        entity->position.y += (int)deltaY;
    }

    if (fitToBounds)
    {
        auto cameraBounds = Engine::RectI{
            entity->position.x - screenSize.x / 2,
            entity->position.y - screenSize.y / 2,
            screenSize.x, screenSize.y};

        cameraBounds.fitInto(bounds);
        entity->position.x = cameraBounds.center_x();
        entity->position.y = cameraBounds.center_y();
    }
}

void CameraComponent::setBounds(const Engine::RectI &bounds)
{
    this->bounds = bounds;
    shouldFitToBounds(true);
}
void CameraComponent::shouldFitToBounds(bool value)
{
    this->fitToBounds = value;
}

const glm::mat3x2 &CameraComponent::getMatrix()
{
    // 2 is the translation column
    matrix[2].x = -entity->position.x + screenSize.x / 2.0;
    matrix[2].y = -entity->position.y + screenSize.y / 2.0;
    return matrix;
}

void CameraComponent::follow(Engine::Entity *e)
{
    this->targetEntity = e;
}

void CameraComponent::render(Engine::Batch &batch)
{
}

CameraComponent::CameraComponent(const glm::ivec2 &screenSize, const glm::vec2 &offset) : screenSize{screenSize}, offset{offset}
{
}

CameraComponent::CameraComponent(const glm::ivec2 &screenSize, const glm::vec2 &offset, const glm::vec2 &inertia) : screenSize{screenSize}, offset{offset}, inertia{inertia}
{

    if (inertia.x > 1.0f)
        this->inertia.x = 1.0f;
    if (inertia.y > 1.0f)
        this->inertia.y = 1.0f;

    if (inertia.x < 0.0f)
        this->inertia.x = 0.0f;
    if (inertia.y < 0.0f)
        this->inertia.y = 0.0f;
}

CameraComponent::~CameraComponent()
{
    targetEntity = nullptr;
}
