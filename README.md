# learn-opengl

Examples can be compiled on linux using:

cc -o core_basic_triangle examples/core_basic_triangle.c -lSDL2 -lGLEW -lGL

or, when using cglm (glm but for C):
cc -o core_basic_triangle examples/core_basic_triangle.c -lSDL2 -lGLEW -lGL -lcglm -lm

Given that you have the dependecies installed: libsdl2 and libglew

basic_coordinate_system result:

![](https://s4.ezgif.com/tmp/ezgif-4-ebe266fdec.gif)