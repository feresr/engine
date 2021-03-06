#include "Engine.h"
#include "Background.h"
#include "Bird.h"
#include "Pipe.h"
#include "Floor.h"
#include "Slider.h"

class SandboxApplication : public Engine::Application
{

private:
    Engine::Batch batch;
    static constexpr int WIDTH = 288;
    static constexpr int HEIGHT = 512;
    static constexpr float SCALE = 1.7;
    static constexpr int PIPE_INTERVAL_MS = 2400;

    std::shared_ptr<Engine::FrameBuffer> buffer;
    std::shared_ptr<Engine::Material> material;
    Engine::Entity *player;
    bool gameover = false;

public:
    SandboxApplication() : Engine::Application("Flappy Bird", WIDTH * SCALE, HEIGHT * SCALE, false)
    {
        buffer = Engine::FrameBuffer::create(WIDTH, HEIGHT);
        auto shader = Engine::Shader::create("assets/ctr.vsh", "assets/ctr.fsh");
        material = std::shared_ptr<Engine::Material>(new Engine::Material(shader));
        batch.defaultSampler = Engine::TextureSampler(Engine::TextureFilter::Nearest);

        world.addEntity({0.0f, 0.0f})->add<Background>(WIDTH, HEIGHT);
        world.addEntity({0.0f, 0.0f})->add<Floor>(WIDTH, HEIGHT);
        world.addEntity({80.0f, HEIGHT / 2.0f})->add<Bird>().onPipeCrash = [=]()
        { gameover = true; };
    }

    void createPipes()
    {
        static constexpr int MIN_GAP_SIZE = 40;
        static constexpr int Y_OFFSET = 30.0f;
        // top
        {
            auto pipe = world.addEntity({0.0f, 0.0f});
            pipe->add<Pipe>(false);
            pipe->position.y = (HEIGHT / 2.0f - Y_OFFSET) + (MIN_GAP_SIZE + rand() % 100);
            pipe->position.x = WIDTH + pipe->get<Engine::SpriteComponent>()->getCurrentAnimSize().x;
        }

        // bottom
        {
            auto pipe = world.addEntity({0.0f, 0.0f});
            pipe->add<Pipe>(true);
            pipe->position.y = (HEIGHT / 2.0f - Y_OFFSET) - (MIN_GAP_SIZE + rand() % 100);
            pipe->position.x = WIDTH + pipe->get<Engine::SpriteComponent>()->getCurrentAnimSize().x;
        }
    }

    void update() override
    {
        world.update();

        // Remove pipes when they go out of screen
        auto pipes = world.componentsOfType<Pipe>();
        for (auto pipe : pipes)
        {
            auto pipeWidth = pipe->getEntity()->get<Engine::SpriteComponent>()->getCurrentAnimSize().x / 2.0f;
            if (pipe->getEntity()->position.x < -pipeWidth)
                world.destroyEntity(pipe->getEntity());
        }

        // Create new pipess every PIPE_INTERVAL_MS
        static uint32_t lastTime = 0;
        if (!gameover && Engine::Time::getTime() - lastTime > PIPE_INTERVAL_MS)
        {
            lastTime = Engine::Time::getTime();
            createPipes();
        }

        // Stop the world on gameover
        world.first<Bird>()->dead = gameover;
        for (auto slider : world.componentsOfType<Slider>())
            slider->velocity = gameover ? 0 : 1;

        // Reload game
        if (Engine::Input::pressed(Engine::R))
        {
            gameover = false;
            world.first<Bird>()->position().y = HEIGHT / 2;
            world.first<Bird>()->get<Kinetic>()->speed.y = -5.0f;
        }
    }

    void render() override
    {
        buffer->clear();
        batch.pushMaterial(material);
        world.render<Background>(batch);
        batch.popMaterial();
        batch.pushBlend(Engine::BlendMode::Normal);
        world.render<Pipe>(batch);
        world.render<Bird>(batch);
        world.render<Floor>(batch);
        batch.render(buffer);
        batch.popBlend();
        batch.clear();

        // Render to screen
        auto screenBuffer = Engine::FrameBuffer::BackBuffer();
        screenBuffer->clear(0xffffffff);
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