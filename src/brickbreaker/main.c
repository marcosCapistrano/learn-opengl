#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "cglm/cglm.h"

#include "paddle.h"

#define TARGET_FPS 6000
#define MS_PER_FRAME 1000 / TARGET_FPS

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

bool isRunning = false;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_GLContext *context;

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Error initializing SDL: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Dodge This", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL);
    if (!window)
    {
        SDL_Log("Error creating SDL Window: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    context = SDL_GL_CreateContext(window);
    if (!context)
    {
        SDL_Log("Error creating GL Context: %s", SDL_GetError());
    }

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        SDL_Log("Error initing glew: %s", glewGetErrorString(err));
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    isRunning = true;
}

int main()
{
    if (init() != 0)
    {
        return 1;
    }

    mat4 view;
    mat4 projection;
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);
    glm_translate(view, (vec3){0.0f, 0.0f, -50.0f});

    Paddle_init();

    Uint64 msPrevFrame = 0;
    int frameCount = 0;
    float fps = 0.0f;

    while (isRunning)
    {
        Uint64 timeToWait = MS_PER_FRAME - (SDL_GetTicks64() - msPrevFrame);

        if (timeToWait > 0 && timeToWait <= MS_PER_FRAME)
            SDL_Delay(timeToWait);

        double deltaTime = (double)(SDL_GetTicks64() - msPrevFrame) / 1000.0;

        msPrevFrame = SDL_GetTicks64();

        if (deltaTime != 0)
            fps = 1.0f / deltaTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                isRunning = false;
            }
            break;

            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
            }
            break;
            }
        }

        SDL_PumpEvents();

        int arrayLen;
        Uint8* keyStates = SDL_GetKeyboardState(&arrayLen);
        if(keyStates[SDL_SCANCODE_LEFT] && keyStates[SDL_SCANCODE_RIGHT]) {
            Paddle_setDir(0);
        } else if(keyStates[SDL_SCANCODE_LEFT]) {
            Paddle_setDir(-1);
        } else if(keyStates[SDL_SCANCODE_RIGHT]) {
            Paddle_setDir(1);
        } else {
            Paddle_setDir(0);
        }

        Paddle_update(deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Paddle_draw(&view, &projection);

        SDL_GL_SwapWindow(window);
        SDL_Log("fps: %.2f", fps);
    }

    return 0;
}