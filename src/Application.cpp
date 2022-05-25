#include "Application.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include <utility>
#include <Log.h>
#include <glad/glad.h>

#include "time/time.h"
#include "Content.h"
#include "Input.h"

Engine::Application *Engine::Application::instance = nullptr;
Engine::Application::Application(std::string name, int width, int height, bool fullScreen) : appName{std::move(name)} {

    // Init platform
    {
        // Get SDL version
        SDL_version version;
        SDL_GetVersion(&version);

        ENGINE_CORE_INFO("SDL v{:d}.{:d}.{:d}", major, version.minor, version.patch);

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        Uint32 flags = fullScreen ? SDL_WINDOW_FULLSCREEN : 0;
        window = SDL_CreateWindow(appName.c_str(),
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  flags | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if (!window) {
            SDL_Log("Unable to create window SDL: %s", SDL_GetError());
            SDL_Quit();
            return;
        }

        // set window properties
        SDL_SetWindowResizable(window, SDL_TRUE);
        SDL_SetWindowMinimumSize(window, 128, 128);

        // enable v-sync
        SDL_GL_SetSwapInterval(1);

        isRunning = true;
        instance = this;
    }

    // Init graphics
    {
        context = SDL_GL_CreateContext(window);

        if (context == nullptr) ENGINE_ERROR("Failed to create OpenGL context: {}", SDL_GetError());
        SDL_GL_MakeCurrent(window, context);
        if (!gladLoadGL()) std::cout << "Failed to initialize GLAD" << std::endl;

        //TODO: add a debug callback

        ENGINE_CORE_INFO(
                "OpenGL {}, {}",
                glGetString(GL_VERSION),
                glGetString(GL_RENDERER)
        );
    }
    // Init ImGui
    {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void) io;
            ImGui::StyleColorsDark();
            ImGui_ImplSDL2_InitForOpenGL(window,context);
            const char* glslversion = "#version 330";
            ImGui_ImplOpenGL3_Init(glslversion);
    }
    Content::load();
}

Engine::Application::~Application() {
    SDL_GL_DeleteContext(context);
    if (window) SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit();
    ENGINE_INFO("GAME CLEANED");
}

void Engine::Application::run() {
    uint32_t frameStart, frameTime;

    while (isRunning) {
        frameStart = SDL_GetTicks();

        // Poll system events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) isRunning = false;
            if (event.type == SDL_QUIT) isRunning = false;
            handleEvent(event);
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        update();
        render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // present
        SDL_GL_SwapWindow(window);
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DURATION + Engine::Time::pauseTimer) {
            SDL_Delay(FRAME_DURATION - frameTime + Engine::Time::pauseTimer);
            Engine::Time::pauseTimer = 0;
        }
        Input::update();
    }

    // shutdown graphics?
    // shutdown platform?
    // shut down imgui
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

}

const char *Engine::Application::path() {
    return SDL_GetBasePath();
}
