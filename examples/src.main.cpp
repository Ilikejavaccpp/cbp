/* This is an example of a raylib application */
#include <raylib.h>

namespace pallete {
    Color ARCH_GRAY = { 162, 162, 208, 255 };
}

int main()
{
    InitWindow(800, 600, "My Window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(pallete::ARCH_GRAY);

        DrawText("Hello Raylib!", 0, 0, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
