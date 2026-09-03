#include "raylib.h"

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Solucionador de Ladrilhos de Wang");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(
            "Solucionador de Ladrilhos de Wang",
            20,
            20,
            30,
            BLACK
        );

        DrawText(
            "Raylib funcionando!",
            20,
            70,
            20,
            DARKGRAY
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
