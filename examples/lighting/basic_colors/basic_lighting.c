#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "cglm/cglm.h"

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

    void *vertexShaderSource = SDL_LoadFile("./shaders/common.vert", NULL);
    void *fragmentShaderSource = SDL_LoadFile("./shaders/objects.frag", NULL);
    void *lightFragmentShaderSource = SDL_LoadFile("./shaders/light.frag", NULL);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const char **)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        SDL_Log("Vertex shader (object) compile error: %s\n", infoLog);
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char **)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        SDL_Log("Frag shader (object) compile error: %s\n", infoLog);
    }

    // link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        SDL_Log("Shader program (object) linking failed: %s\n", infoLog);
    }
    glDeleteShader(fragmentShader);

    // fragment shader
    unsigned int lightFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(lightFragmentShader, 1, (const char **)&lightFragmentShaderSource, NULL);
    glCompileShader(lightFragmentShader);

    // check for shader compile errors
    glGetShaderiv(lightFragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(lightFragmentShader, 512, NULL, infoLog);
        SDL_Log("Frag shader (light) compile error: %s\n", infoLog);
    }

    // link shaders
    GLuint lightShaderProgram = glCreateProgram();
    glAttachShader(lightShaderProgram, vertexShader);
    glAttachShader(lightShaderProgram, lightFragmentShader);
    glLinkProgram(lightShaderProgram);

    // check for linking errors
    glGetProgramiv(lightShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(lightShaderProgram, 512, NULL, infoLog);
        SDL_Log("Shader program (light) linking failed: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(lightFragmentShader);

    SDL_free(vertexShaderSource);
    SDL_free(fragmentShaderSource);
    SDL_free(lightFragmentShaderSource);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 3 * sizeof(float));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Preparation for the shaders
    mat4 projection;
    glm_mat4_identity(projection);
    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

    /* Sets UP the shader for the objects in the scene */
    glUseProgram(shaderProgram);

    // Lighting Uniforms
    unsigned int objColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    glUniform3fv(objColorLoc, 1, (vec3){1.0f, 0.5f, 0.31f});
    glUniform3fv(lightColorLoc, 1, (vec3){1.0f, 1.0f, 1.0f});
    // ------------------------------------------------------------

    // Projection and model matrix
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, 0.0f, 0.0f});

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    // ------------------------------------------------------------

    // Sets UP the shader for the light in the scene
    glUseProgram(lightShaderProgram);

    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)projection);

    glm_mat4_identity(model);
    glm_translate(model, (vec3){3.0f, 3.0f, 0.0f});
    glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});

    modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    // ------------------------------

    printf("got here");
    while (isRunning)
    {
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

                if (event.key.keysym.sym == SDLK_d)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
            }
            break;
            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_d)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
            break;
            }
        }

        mat4 view;
        glm_mat4_identity(view);

        const float radius = 10.0f;
        float camX = sin(SDL_GetTicks64() / 1000.0f) * radius;
        float camZ = cos(SDL_GetTicks64() / 1000.0f) * radius;

        glm_lookat((vec3){camX, 2.0f, camZ}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

        glClearBufferfv(GL_COLOR, 0, (vec4){0.2f, 0.3f, 0.3f, 1.0f});

        glUseProgram(shaderProgram);

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);

        glBindVertexArray(VAO);                               // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 36); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!

        // draw our first triangle
        glUseProgram(lightShaderProgram);

        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);

        glBindVertexArray(lightVAO);                          // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 36); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!
        // glBindVertexArray(0); // no need to unbind it every time

        SDL_GL_SwapWindow(window);
    }

    return 0;
}