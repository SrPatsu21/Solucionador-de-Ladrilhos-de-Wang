#ifndef RENDERER_H
#define RENDERER_H

#include "tile.h"
#include "tm.h"

void rendererDrawTile(
    const Tile *tile,
    int x,
    int y,
    int size
);

void rendererDrawTM(
    const TMExecution *execution,
    int x,
    int y,
    int cellSize
);

void rendererHandleTMControls(
    TMExecution *execution
);

#endif