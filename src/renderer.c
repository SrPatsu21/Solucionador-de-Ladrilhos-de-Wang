#include "renderer.h"

#include "raylib.h"

#include <stddef.h>
#include <stdio.h>

#define TILE_BORDER_THICKNESS 6

static Color tileColorToRaylib(TileColor color)
{
    switch (color) {
        case TILE_COLOR_RED:
            return RED;

        case TILE_COLOR_GREEN:
            return GREEN;

        case TILE_COLOR_BLUE:
            return BLUE;

        case TILE_COLOR_YELLOW:
            return YELLOW;

        case TILE_COLOR_NONE:
        default:
            return GRAY;
    }
}

void rendererDrawTile(
    const Tile *tile,
    int x,
    int y,
    int size
)
{
    if (tile == NULL) {
        return;
    }

    Color topColor = tileColorToRaylib(tile->top);
    Color rightColor = tileColorToRaylib(tile->right);
    Color bottomColor = tileColorToRaylib(tile->bottom);
    Color leftColor = tileColorToRaylib(tile->left);

    DrawRectangle(
        x,
        y,
        size,
        size,
        LIGHTGRAY
    );

    DrawRectangle(
        x,
        y,
        size,
        TILE_BORDER_THICKNESS,
        topColor
    );

    DrawRectangle(
        x + size - TILE_BORDER_THICKNESS,
        y,
        TILE_BORDER_THICKNESS,
        size,
        rightColor
    );

    DrawRectangle(
        x,
        y + size - TILE_BORDER_THICKNESS,
        size,
        TILE_BORDER_THICKNESS,
        bottomColor
    );

    DrawRectangle(
        x,
        y,
        TILE_BORDER_THICKNESS,
        size,
        leftColor
    );

    DrawRectangleLines(
        x,
        y,
        size,
        size,
        BLACK
    );

    char idText[32];

    snprintf(
        idText,
        sizeof(idText),
        "%d",
        tile->id
    );

    int fontSize = size / 3;

    if (fontSize < 10) {
        fontSize = 10;
    }

    int textWidth = MeasureText(idText, fontSize);

    DrawText(
        idText,
        x + (size - textWidth) / 2,
        y + (size - fontSize) / 2,
        fontSize,
        BLACK
    );
}