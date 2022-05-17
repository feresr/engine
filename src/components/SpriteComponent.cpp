#include <Application.h>
#include <math/Utils.h>
#include "SpriteComponent.h"
#include "Ecs.h"
#include "Sprite.h"
#include "Content.h"
#include "Batch.h"

Engine::SpriteComponent::SpriteComponent(const std::string &spriteName) : spriteName{spriteName}
{
}

bool Engine::SpriteComponent::awake()
{
    return Component::awake();
}

void Engine::SpriteComponent::play(const std::string &animationName)
{
    if (this->animationName != animationName)
    {
        frameIndex = 0;
        frameCounter = 0.0f;
        this->animationName = animationName;
    }
}

void Engine::SpriteComponent::render(Engine::Batch &batch)
{
    auto &texture = getAnimation()->frames[frameIndex].texture;
    batch.pushMatrix(Engine::Math::transform(entity->position, getSprite()->pivot, scale));
    batch.tex(texture, glm::vec2(0), Engine::Color(0xffffff));
    batch.popMatrix();
}

void Engine::SpriteComponent::update()
{
    frameCounter += FRAME_DURATION;
    if (frameCounter > getAnimation()->frames[frameIndex].durationMillis)
    {
        frameIndex++;
        frameCounter = 0.0f;
        if (frameIndex >= getAnimation()->frames.size())
            frameIndex = 0;
    }
}

int Engine::SpriteComponent::getCurrentAnimDuration()
{
    return getAnimation()->duration;
}

// todo: getCurrentAnimSize doesn't work becaue the packer packs alpha values
glm::ivec2 Engine::SpriteComponent::getCurrentAnimSize()
{
    auto texture = getAnimation()->frames[frameIndex].texture;
    return glm::ivec2{texture.width(), texture.height()};
}

Engine::Sprite *Engine::SpriteComponent::getSprite()
{
    return Content::findSprite(spriteName);
}

Engine::Animation *Engine::SpriteComponent::getAnimation()
{
    return getSprite()->getAnimation(animationName);
}