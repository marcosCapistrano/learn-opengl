#ifndef PADDLE_INCLUDED
#define PADDLE_INCLUDED

void Paddle_init();
void Paddle_update(float deltaTime);
void Paddle_draw(mat4 *view, mat4 *projection);
void Paddle_setDir(int dir);

#endif