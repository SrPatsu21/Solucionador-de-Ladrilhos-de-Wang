#include "renderer.h"

#include "raylib.h"

#include <stddef.h>
#include <stdio.h>

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

    int centerX = x + size / 2;
    int centerY = y + size / 2;

    /*
     * Fundo do ladrilho
     */
    DrawRectangle(
        x,
        y,
        size,
        size,
        LIGHTGRAY
    );

    /*
     * TRIÂNGULO SUPERIOR
     *
     *        RED
     *     ───────
     *      \   /
     *       \ /
     *        ▼
     */
    DrawTriangle(
        (Vector2){
            x,
            y
        },
        (Vector2){
            centerX,
            centerY
        },
        (Vector2){
            x + size,
            y
        },
        topColor
    );

    /*
     * TRIÂNGULO DIREITO
     *
     *          BLUE
     *            >
     *           / \
     *          /   \
     *         /     \
     */
    DrawTriangle(
        (Vector2){
            x + size,
            y
        },
        (Vector2){
            centerX,
            centerY
        },
        (Vector2){
            x + size,
            y + size
        },
        rightColor
    );

    /*
     * TRIÂNGULO INFERIOR
     *
     *        ▲
     *       / \
     *      /   \
     *     ───────
     *      GREEN
     */
    DrawTriangle(
        (Vector2){
            x,
            y + size
        },
        (Vector2){
            x + size,
            y + size
        },
        (Vector2){
            centerX,
            centerY
        },
        bottomColor
    );

    /*
     * TRIÂNGULO ESQUERDO
     *
     *      YELLOW
     *        <
     *       / \
     *      /   \
     *     /     \
    */
    DrawTriangle(
        (Vector2){
            x,
            y
        },
        (Vector2){
            x,
            y + size
        },
        (Vector2){
            centerX,
            centerY
        },
        leftColor
    );

    /*
     * Borda externa do ladrilho
     */
    DrawRectangleLines(
        x,
        y,
        size,
        size,
        BLACK
    );

    /*
     * ID do ladrilho
     */
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

    int textWidth = MeasureText(
        idText,
        fontSize
    );

    DrawText(
        idText,
        x + (size - textWidth) / 2,
        y + (size - fontSize) / 2,
        fontSize,
        BLACK
    );
}