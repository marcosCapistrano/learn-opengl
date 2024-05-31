#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "cglm/cglm.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

bool isRunning = false;
static SDL_Window *window;
static SDL_GLContext *context;

typedef struct Material
{
    float shininess;
} Material;

typedef struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
} Light;

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

    return 0;
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

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    // world space positions of our cubes
    vec3 cubePositions[10] = {
        {0.0f, 0.0f, 0.0f},
        {3.0f, 5.0f, -15.0f},
        {-3.5f, -2.2f, -2.5f},
        {-4.8f, -2.0f, -12.3f},
        {4.4f, -0.4f, -3.5f},
        {-3.7f, 3.0f, -7.5f},
        {3.3f, -2.0f, -2.5f},
        {3.5f, 2.0f, -2.5f},
        {3.5f, 0.2f, -1.5f},
        {-3.3f, 1.0f, -1.5f}};

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    SDL_Surface *surface = IMG_Load("./resources/container.png");
    if (!surface)
    {
        SDL_Log("Error loading image: %s", IMG_GetError());
    }

    unsigned int textures[2];
    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_LockSurface(surface);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_UnlockSurface(surface);

    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);

    surface = IMG_Load("./resources/container_specular.png");

    if (!surface)
    {
        SDL_Log("Error loading image: %s", IMG_GetError());
    }

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_LockSurface(surface);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_UnlockSurface(surface);

    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.specular"), 1);

    // Preparation for the shaders
    Material material = {
        76.8f};

    Light light = {
        {0.0f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.2f},
        {0.5f, 0.5f, 0.5f},
        {1.0f, 1.0f, 1.0f},
        1.0f,
        0.09f,
        0.032f};

    vec3 viewPos = {-5.0f, 2.0f, -5.0f};

    mat4 projection;
    glm_mat4_identity(projection);
    glm_perspective(glm_rad(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

    mat4 view;
    glm_mat4_identity(view);
    glm_lookat(viewPos, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

    /* Sets UP the shader for the objects in the scene */
    glUseProgram(shaderProgram);

    // Lighting Uniforms
    unsigned int matShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

    unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "light.position");
    unsigned int lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
    unsigned int lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
    unsigned int lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");

    unsigned int lightConstantLoc = glGetUniformLocation(shaderProgram, "light.constant");
    unsigned int lightLinearLoc = glGetUniformLocation(shaderProgram, "light.linear");
    unsigned int lightQuadraticLoc = glGetUniformLocation(shaderProgram, "light.quadratic");

    unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    glUniform1f(matShininessLoc, material.shininess);

    glUniform3fv(lightPosLoc, 1, light.position);
    glUniform3fv(lightAmbientLoc, 1, light.ambient);
    glUniform3fv(lightDiffuseLoc, 1, light.diffuse);
    glUniform3fv(lightSpecularLoc, 1, light.specular);

    glUniform1f(lightConstantLoc, light.constant);
    glUniform1f(lightLinearLoc, light.linear);
    glUniform1f(lightQuadraticLoc, light.quadratic);

    glUniform3fv(viewPosLoc, 1, viewPos);
    // ------------------------------------------------------------

    // Projection and model matrix
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)projection);

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);

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

    viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);

    glm_mat4_identity(model);
    glm_translate(model, light.position);
    glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});

    modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
    // ------------------------------

    glEnable(GL_DEPTH_TEST);

    float lightRotationH = 0;
    float lightRotationV = 0;
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

        SDL_PumpEvents();

        int mod = 1;
        int arrayLen;
        const Uint8 *keyStates = SDL_GetKeyboardState(&arrayLen);
        if (keyStates[SDL_SCANCODE_SPACE])
        {
            mod = 3;
        }
        if (keyStates[SDL_SCANCODE_LEFT])
        {
            lightRotationH -= 0.01 * mod;
        }
        else if (keyStates[SDL_SCANCODE_RIGHT])
        {
            lightRotationH += 0.01 * mod;
        }

        if (keyStates[SDL_SCANCODE_UP])
        {
            lightRotationV += 0.01 * mod;
        }
        else if (keyStates[SDL_SCANCODE_DOWN])
        {
            lightRotationV -= 0.01 * mod;
        }

        const float radiusH = 2.0f;
        const float radiusV = 2.0f;
        float lightX = sin(lightRotationH) * radiusH;
        float lightY = sin(lightRotationV) * radiusV;
        float lightZ = cos(lightRotationH) * radiusH;

        light.position[0] = lightX;
        light.position[1] = lightY;
        light.position[2] = lightZ;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "light.position");
        glUniform3fv(lightPosLoc, 1, light.position);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            mat4 model;
            glm_mat4_identity(model);

            glm_translate(model, cubePositions[i]);

            float angle = 20.0f * i + 20.0f;

            glm_rotate(model, glm_rad(angle + (SDL_GetTicks64() / 100.0f) * (i + 1)), (vec3){1.0f, 0.3f, 0.5f});
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw our first triangle
        glUseProgram(lightShaderProgram);

        glm_mat4_identity(model);
        glm_translate(model, light.position);
        glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});

        modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glBindVertexArray(lightVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0); // no need to unbind it every time

        SDL_GL_SwapWindow(window);
    }

    return 0;
}