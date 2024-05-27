#include "GL/glew.h"
#include "cglm/cglm.h"
#include "SDL2/SDL.h"

static float vertices[] = {
    10.0f, 0.5f, 0.0f,   // top right
    10.0f, -0.5f, 0.0f,  // bottom right
    -10.0f, -0.5f, 0.0f, // bottom left
    -10.0f, 0.5f, 0.0f   // top left
};

static unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

typedef struct Paddle
{
    GLuint shaderProgram;
    unsigned int VAO, VBO, EBO;

    vec3 position;
    vec3 speed;
} Paddle;

static Paddle paddle;

void Paddle_init()
{
    // Shader program
    void *vertexShaderSource = SDL_LoadFile("./shaders/paddle.vert", NULL);
    void *fragmentShaderSource = SDL_LoadFile("./shaders/paddle.frag", NULL);

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
        SDL_Log("Vertex shader compile error: %s\n", infoLog);
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
        SDL_Log("Frag shader compile error: %s\n", infoLog);
    }

    // link shaders
    paddle.shaderProgram = glCreateProgram();
    glAttachShader(paddle.shaderProgram, vertexShader);
    glAttachShader(paddle.shaderProgram, fragmentShader);
    glLinkProgram(paddle.shaderProgram);

    // check for linking errors
    glGetProgramiv(paddle.shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(paddle.shaderProgram, 512, NULL, infoLog);
        SDL_Log("Shader program linking failed: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_free(vertexShaderSource);
    SDL_free(fragmentShaderSource);

    glGenVertexArrays(1, &(paddle.VAO));
    glGenBuffers(1, &(paddle.VBO));
    glGenBuffers(1, &(paddle.EBO));

    // Bind and buffer data
    glBindVertexArray(paddle.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, paddle.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paddle.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm_vec3_zero(&paddle.position);
    paddle.position[1] = -20.0f;

    glm_vec3_zero(&paddle.speed);
}

void Paddle_update(float deltaTime)
{
    paddle.position[0] += paddle.speed[0] * deltaTime;
}

void Paddle_draw(mat4 *view, mat4 *projection)
{
    glUseProgram(paddle.shaderProgram);

    unsigned int viewLoc = glGetUniformLocation(paddle.shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(paddle.shaderProgram, "projection");
    unsigned int modelLoc = glGetUniformLocation(paddle.shaderProgram, "model");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)*projection);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)*view);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, paddle.position);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);

    glBindVertexArray(paddle.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!
}

const int PADDLE_SPEED = 10;
void Paddle_setDir(int dir)
{
    if(dir < 0)
    {
        paddle.speed[0] = -PADDLE_SPEED;
    } else if(dir > 0)
    {
        paddle.speed[0] = PADDLE_SPEED;
    } else {
        paddle.speed[0] = 0;
    }
}
