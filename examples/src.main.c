/* This is an example of a raylib application (C version) */
#include <raylib.h>

/* ANSI C programming book recommends you use these instead
 * of a lot of macros
 */
const Color ARCH_GRAY = { 162, 162, 208, 255 };

int main()
{
    InitWindow(800, 600, "My Window");

    Font mono = LoadFont("../assets/fonts/mono.ttf");

    SetTargetFPS(144);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(ARCH_GRAY);

        DrawTextEx(mono, "Hello, World!", (Vector2){0, 14}, 16, 1, BLACK);

        EndDrawing();
    }

    UnloadFont(mono);

    CloseWindow();
    return 0;
}
