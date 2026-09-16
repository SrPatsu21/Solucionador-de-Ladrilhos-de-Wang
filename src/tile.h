#ifndef TILE_H
#define TILE_H

typedef enum {
    TILE_COLOR_NONE = 0,
    TILE_COLOR_RED,
    TILE_COLOR_GREEN,
    TILE_COLOR_BLUE,
    TILE_COLOR_YELLOW
} TileColor;

typedef struct {
    int id;

    TileColor top;
    TileColor right;
    TileColor bottom;
    TileColor left;
} Tile;

Tile tileCreate(
    int id,
    TileColor top,
    TileColor right,
    TileColor bottom,
    TileColor left
);

void tilePrint(const Tile *tile);

#endif