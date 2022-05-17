#include "Engine.h"
#include "Background.h"
#include "Bird.h"

class SandboxApplication : public Engine::Application
{
private:
    Engine::Batch batch;
    static constexpr int WIDTH = 288;
    static constexpr int HEIGHT = 512;
    static constexpr float SCALE = 1.7;

    std::shared_ptr<Engine::FrameBuffer> buffer;
    Engine::Entity* player;

public:
    SandboxApplication() : Engine::Application("Flappy Bird", WIDTH * SCALE, HEIGHT * SCALE, false)
    {
        buffer = Engine::FrameBuffer::create(WIDTH, HEIGHT);
        batch.defaultSampler = Engine::TextureSampler(Engine::TextureFilter::Nearest);

        world.addEntity({0.0f, 0.0f})->add<Background>(WIDTH, HEIGHT);

        player = world.addEntity({80.0f, HEIGHT / 2.0f});
        player->add<Bird>();
    }

    void update() override
    {
        world.update();
    }

    void render() override
    {
        Engine::FrameBuffer::BackBuffer()->clear(0x000000);
        world.render<Background>(batch);
        world.render<Engine::SpriteComponent>(batch);
        batch.render(buffer);
        batch.clear();

        // Render to screen
        auto screenBuffer = Engine::FrameBuffer::BackBuffer();
        glm::vec2 screenCenter = glm::vec2{(float)screenBuffer->width(), (float)screenBuffer->height()} * 0.5f;
        glm::vec2 bufferCenter = glm::vec2{(float)buffer->width(), (float)buffer->height()} * 0.5f;
        glm::vec2 scale = {screenBuffer->width() / (float)buffer->width(), screenBuffer->height() / (float)buffer->height()};
        batch.pushMatrix(Engine::Math::transform(screenCenter, bufferCenter, scale));
        batch.tex(buffer->attachment(0), {0.0f, 0.0f}, 0xffffff);
        batch.render(Engine::FrameBuffer::BackBuffer());
        batch.popMatrix();
        batch.clear();
    }

    void handleEvent(SDL_Event &event) override
    {
    }
};

Engine::Application *Engine::createApplication()
{
    return new SandboxApplication();
}