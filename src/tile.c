#include "tile.h"

#include <stdio.h>

Tile tileCreate(
    int id,
    TileColor top,
    TileColor right,
    TileColor bottom,
    TileColor left
)
{
    Tile tile;

    tile.id = id;
    tile.top = top;
    tile.right = right;
    tile.bottom = bottom;
    tile.left = left;

    return tile;
}

void tilePrint(const Tile *tile)
{
    if (tile == NULL) {
        return;
    }

    printf(
        "Tile %d: top=%d right=%d bottom=%d left=%d\n",
        tile->id,
        tile->top,
        tile->right,
        tile->bottom,
        tile->left
    );
}