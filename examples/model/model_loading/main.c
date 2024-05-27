#include <stdio.h>
#include <ctype.h>

int main()
{
    FILE *objFile = fopen("./resources/landscape.obj", "r");
    if (!objFile)
    {
        perror("fopen failed");
        return 1;
    }

    FILE *mtlFile = fopen("./resources/landscape.obj", "r");
    if (!mtlFile)
    {
        perror("fopen failed");
        return 1;
    }

    char line[100];
    while ((fgets(line, 100, objFile) != NULL))
    {
            printf("line: %s\n", line);
    }

    printf("ok\n");

    return 0;
}