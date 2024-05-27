#include "GL/glew.h"
#include "cglm/cglm.h"
#include "SDL2/SDL.h"

static float vertices[] = {
    0.0f, 0.0f, 0.0f
};

typedef struct Ball
{
    GLuint shaderProgram;
    unsigned int VAO, VBO, EBO;

    vec3 position;
    vec3 speed;
} Ball;

static Ball ball;

void Ball_init()
{
    // Shader program
    void *vertexShaderSource = SDL_LoadFile("./shaders/ball.vert", NULL);
    void *fragmentShaderSource = SDL_LoadFile("./shaders/ball.frag", NULL);

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
    ball.shaderProgram = glCreateProgram();
    glAttachShader(ball.shaderProgram, vertexShader);
    glAttachShader(ball.shaderProgram, fragmentShader);
    glLinkProgram(ball.shaderProgram);

    // check for linking errors
    glGetProgramiv(ball.shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ball.shaderProgram, 512, NULL, infoLog);
        SDL_Log("Shader program linking failed: %s\n", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_free(vertexShaderSource);
    SDL_free(fragmentShaderSource);

    glGenVertexArrays(1, &(ball.VAO));
    glGenBuffers(1, &(ball.VBO));

    // Bind and buffer data
    glBindVertexArray(ball.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, ball.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm_vec3_zero(&ball.position);
    ball.position[1] = 10.0f;

    glm_vec3_zero(&ball.speed);
}

void Ball_update(float deltaTime)
{
    ball.position[0] += ball.speed[0] * deltaTime;
}

void Ball_draw(mat4 *view, mat4 *projection)
{
    glUseProgram(ball.shaderProgram);

    unsigned int viewLoc = glGetUniformLocation(ball.shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(ball.shaderProgram, "projection");
    unsigned int modelLoc = glGetUniformLocation(ball.shaderProgram, "model");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float *)*projection);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)*view);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, ball.position);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);

glPointSize(10.0f);
    glBindVertexArray(ball.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_POINTS, 0, 1); // set the count to 6 since we're drawing 6 vertices now (2 triangles); not 3!
}

const int BALL_SPEED = 10;
void Ball_setDir(int dir)
{
    if(dir < 0)
    {
        ball.speed[0] = -BALL_SPEED;
    } else if(dir > 0)
    {
        ball.speed[0] = BALL_SPEED;
    } else {
        ball.speed[0] = 0;
    }
}
