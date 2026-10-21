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

void rendererDrawTM(
    const TMExecution *execution,
    int x,
    int y,
    int cellSize
)
{
    if (
        execution == NULL ||
        execution->machine == NULL ||
        execution->tapes == NULL
    )
    {
        return;
    }

    if (
        execution->activeTape < 0 ||
        execution->activeTape >=
            execution->machine->tapeCount
    )
    {
        return;
    }

    const TMTape *tape =
        &execution->tapes[
            execution->activeTape
        ];

    if (
        tape->cells == NULL ||
        tape->size == 0
    )
    {
        return;
    }

    /*
     * Quantas células cabem horizontalmente
     * na tela.
     */
    int screenWidth = GetScreenWidth();

    int visibleCells =
        screenWidth / cellSize;

    if (visibleCells < 1) {
        return;
    }

    /*
     * Não precisamos desenhar mais células
     * do que a própria fita possui.
     */
    size_t cellsToDraw = tape->size;

    if (
        cellsToDraw >
        (size_t)visibleCells
    )
    {
        cellsToDraw = (size_t)visibleCells;
    }

    /*
     * Define a primeira posição da fita que
     * será desenhada.
     *
     * Quando o cabeçote está no começo,
     * começamos no índice 0.
     *
     * Depois disso tentamos manter o
     * cabeçote no centro da tela.
     */
    size_t firstCell = 0;

    if (
        tape->size >
        (size_t)visibleCells
    )
    {
        size_t half =
            (size_t)visibleCells / 2;

        if (tape->head > half)
        {
            firstCell =
                tape->head - half;
        }

        /*
         * Não podemos ultrapassar o final
         * da fita.
         */
        size_t lastPossibleFirst =
            tape->size -
            (size_t)visibleCells;

        if (firstCell > lastPossibleFirst)
        {
            firstCell =
                lastPossibleFirst;
        }
    }

    /*
     * x representa o centro horizontal
     * da fita.
     */
    int totalWidth =
        (int)cellsToDraw * cellSize;

    int startX =
        x - totalWidth / 2;

    /*
     * Desenha as células.
     */
    for (
        size_t i = 0;
        i < cellsToDraw;
        i++
    )
    {
        size_t tapeIndex =
            firstCell + i;

        int cellX =
            startX +
            (int)i * cellSize;

        DrawRectangle(
            cellX,
            y,
            cellSize,
            cellSize,
            RAYWHITE
        );

        DrawRectangleLines(
            cellX,
            y,
            cellSize,
            cellSize,
            BLACK
        );

        /*
         * Símbolo da célula.
         */
        char symbol[2];

        symbol[0] =
            tape->cells[tapeIndex];

        symbol[1] =
            '\0';

        int fontSize =
            cellSize / 2;

        if (fontSize < 10)
        {
            fontSize = 10;
        }

        int textWidth =
            MeasureText(
                symbol,
                fontSize
            );

        DrawText(
            symbol,
            cellX +
                (cellSize - textWidth) / 2,
            y +
                (cellSize - fontSize) / 2,
            fontSize,
            BLACK
        );

        /*
         * Cabeçote.
         */
        if (
            tapeIndex ==
            tape->head
        )
        {
            DrawRectangleLinesEx(
                (Rectangle){
                    cellX + 2,
                    y + 2,
                    cellSize - 4,
                    cellSize - 4
                },
                4,
                RED
            );

            /*
             * Pequena indicação acima da
             * célula ativa.
             */
            DrawTriangle(
                (Vector2){
                    cellX +
                        cellSize / 2,
                    y - 12
                },
                (Vector2){
                    cellX +
                        cellSize / 2 - 8,
                    y
                },
                (Vector2){
                    cellX +
                        cellSize / 2 + 8,
                    y
                },
                RED
            );
        }
    }

    /*
     * Informações da fita ativa.
     */
    char tapeText[64];

    snprintf(
        tapeText,
        sizeof(tapeText),
        "Fita: %d    Cabeca: %zu",
        execution->activeTape,
        tape->head
    );

    DrawText(
        tapeText,
        startX,
        y + cellSize + 10,
        20,
        BLACK
    );
}

void rendererHandleTMControls(
    TMExecution *execution,
    WangSolver *solver
)
{
    if (execution == NULL)
    {
        return;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        if (!solver->solved && !solver->failed) {
            wangSolverStep(solver);
        }
        tmStep(execution);
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        wangSolverStepBack(solver);
        tmStepBack(execution);
    }

    if (IsKeyPressed(KEY_R))
    {
        wangSolverReset(solver);
        tmExecutionReset(execution);
    }
}