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

    int triangleSize = size;

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
     *
     * Os dois primeiros vértices
     * ficam na borda superior.
     *
     * O terceiro fica no centro.
     */
    DrawTriangle(
        (Vector2){
            centerX - triangleSize / 2,
            y
        },
        (Vector2){
            centerX + triangleSize / 2,
            y
        },
        (Vector2){
            centerX,
            centerY
        },
        topColor
    );

    /*
     * TRIÂNGULO DIREITO
     *
     *             ────
     *            /    |
     *           /     |
     *          /      |
     *              BLUE
     *
     * Os dois primeiros vértices
     * ficam na borda direita.
     *
     * O terceiro fica no centro.
     */
    DrawTriangle(
        (Vector2){
            x + size,
            centerY - triangleSize / 2
        },
        (Vector2){
            x + size,
            centerY + triangleSize / 2
        },
        (Vector2){
            centerX,
            centerY
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
     *       GREEN
     *
     * Os dois primeiros vértices
     * ficam na borda inferior.
     *
     * O terceiro fica no centro.
     */
    DrawTriangle(
        (Vector2){
            centerX - triangleSize / 2,
            y + size
        },
        (Vector2){
            centerX + triangleSize / 2,
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
     *        |
     *       /|
     *      / |
     *     /  |
     * YELLOW
     *
     * Os dois primeiros vértices
     * ficam na borda esquerda.
     *
     * O terceiro fica no centro.
     */
    DrawTriangle(
        (Vector2){
            x,
            centerY - triangleSize / 2
        },
        (Vector2){
            x,
            centerY + triangleSize / 2
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