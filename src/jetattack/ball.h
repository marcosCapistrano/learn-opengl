#ifndef BALL_INCLUDED
#define BALL_INCLUDED

void Ball_init();
void Ball_update(float deltaTime);
void Ball_draw(mat4 *view, mat4 *projection);
void Ball_setDir(int dir);

#endif