#include "raylib.h"

#include "renderer.h"
#include "tile.h"

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 700;

    InitWindow(
        screenWidth,
        screenHeight,
        "Solucionador de Ladrilhos de Wang"
    );

    SetTargetFPS(60);

    Tile tile = tileCreate(
        1,
        TILE_COLOR_RED,
        TILE_COLOR_BLUE,
        TILE_COLOR_GREEN,
        TILE_COLOR_YELLOW
    );

    tilePrint(&tile);

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

        rendererDrawTile(
            &tile,
            100,
            150,
            200
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}