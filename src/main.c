#include "raylib.h"

#include "renderer.h"
#include "tile.h"
#include "tm.h"
#include "solver.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("[DEBUG] Programa iniciado.\n");

    const int screenWidth = 1200;
    const int screenHeight = 720;

    printf("[DEBUG] Inicializando janela...\n");

    InitWindow(
        screenWidth,
        screenHeight,
        "Solucionador de Ladrilhos de Wang (Backtracking)"
    );

    printf("[DEBUG] Janela inicializada.\n");

    SetTargetFPS(60);

    /*
     * 1. Criação de um conjunto de ladrilhos (TileSet) de exemplo
     */
    const size_t tileCount = 3;
    Tile *tileSet = malloc(tileCount * sizeof(Tile));

    tileSet[0] = tileCreate(1, TILE_COLOR_RED, TILE_COLOR_GREEN, TILE_COLOR_BLUE, TILE_COLOR_YELLOW);
    tileSet[1] = tileCreate(2, TILE_COLOR_BLUE, TILE_COLOR_GREEN, TILE_COLOR_YELLOW, TILE_COLOR_RED);
    tileSet[2] = tileCreate(3, TILE_COLOR_BLUE, TILE_COLOR_YELLOW, TILE_COLOR_RED, TILE_COLOR_GREEN);

    // tileSet[2] = tileCreate(3, TILE_COLOR_GREEN, TILE_COLOR_RED, TILE_COLOR_YELLOW, TILE_COLOR_GREEN);
    // tileSet[3] = tileCreate(4, TILE_COLOR_YELLOW, TILE_COLOR_GREEN, TILE_COLOR_BLUE, TILE_COLOR_RED);
    // tileSet[4] = tileCreate(5, TILE_COLOR_RED, TILE_COLOR_RED, TILE_COLOR_RED, TILE_COLOR_RED);

    printf("[DEBUG] Conjunto de ladrilhos criado (%zu ladrilhos).\n", tileCount);

    /*
     * 2. Inicialização do Solucionador Wang (Grade 3x3)
     */
    int boardWidth = 3;
    int boardHeight = 3;

    printf("[DEBUG] Inicializando WangSolver (%dx%d)...\n", boardWidth, boardHeight);
    WangSolver solver = wangSolverCreate(boardWidth, boardHeight, tileSet, tileCount);
    printf("[DEBUG] WangSolver inicializado com sucesso.\n");

    /*
     * Carregamento opcional da Máquina de Turing (mantido da sua estrutura original se precisar exibir a fita)
     */
    TuringMachine machine;
    printf("[DEBUG] Carregando maquina de Turing: machine.txt\n");

    bool hasTM = tmLoad("machine.txt", &machine);
    TMTape *tapes = NULL;
    TMExecution execution;
    bool executionCreated = false;

    if (hasTM) {
        printf("[DEBUG] Maquina carregada com sucesso.\n");
        tapes = calloc((size_t)machine.tapeCount, sizeof(TMTape));
        if (tapes != NULL) {
            for (int i = 0; i < machine.tapeCount; i++) {
                tmTapeCreate(&tapes[i], 100);
            }
            if (tmExecutionCreate(&execution, &machine, tapes)) {
                executionCreated = true;
                printf("[DEBUG] Execucao da Maquina de Turing criada.\n");
            }
        }
    } else {
        printf("[AVISO] machine.txt nao encontrado ou invalido. Apenas o solver de ladrilhos sera exibido.\n");
    }

    printf("[DEBUG] Entrando no loop principal.\n");

    while (!WindowShouldClose())
    {
        /*
         * Controles do Solver:
         * SPACE - Executa um passo do backtracking
         * R - Reseta o solver
         */
        // Avançar o solver com SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            if (!solver.solved && !solver.failed) {
                wangSolverStep(&solver);
            }
        }

        // Voltar um passo atrás com BACKSPACE
        if (IsKeyPressed(KEY_BACKSPACE)) {
            wangSolverStepBack(&solver);
        }

        // Reiniciar com R
        if (IsKeyPressed(KEY_R)) {
            wangSolverReset(&solver);
        }

        if (executionCreated) {
            rendererHandleTMControls(&execution);
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(
            "Solucionador de Ladrilhos de Wang - Backtracking",
            20,
            20,
            30,
            BLACK
        );

        /*
         * Renderização do Tabuleiro de Ladrilhos (Grade)
         */
        int tileSize = 100;
        int startBoardX = 100;
        int startBoardY = 100;

        for (int y = 0; y < boardHeight; y++) {
            for (int x = 0; x < boardWidth; x++) {
                int drawX = startBoardX + x * (tileSize + 10);
                int drawY = startBoardY + y * (tileSize + 10);

                Tile *currentTile = solver.board.cells[y * boardWidth + x];

                if (currentTile != NULL) {
                    // Desenha o ladrilho posicionado usando a sua função do renderer.c
                    rendererDrawTile(currentTile, drawX, drawY, tileSize);
                } else {
                    // Desenha uma célula vazia cinza
                    DrawRectangle(drawX, drawY, tileSize, tileSize, LIGHTGRAY);
                    DrawRectangleLines(drawX, drawY, tileSize, tileSize, DARKGRAY);
                    
                    char coordText[16];
                    snprintf(coordText, sizeof(coordText), "%d,%d", x, y);
                    DrawText(coordText, drawX + 25, drawY + 40, 15, DARKGRAY);
                }
            }
        }

        /*
         * Status do Solver na tela
         */
        if (solver.solved) {
            DrawText("STATUS: Solucao encontrada!", 100, 450, 20, GREEN);
        } else if (solver.failed) {
            DrawText("STATUS: Sem solucao para este conjunto!", 100, 450, 20, RED);
        } else {
            DrawText("STATUS: Buscando...", 100, 450, 20, DARKBLUE);
        }

        /*
         * Renderização da Fita da Máquina de Turing (se carregada)
         */
        if (executionCreated) {
            rendererDrawTM(
                &execution,
                screenWidth / 2 + 100,
                550,
                40
            );
        }

        /*
         * Instruções de Teclado
         */
        DrawText("BACKSPACE - Voltar passo anterior | SPACE - Proximo passo do Solver", 20, 630, 20, BLACK);
        DrawText("R - Reiniciar Solver", 20, 660, 20, BLACK);

        EndDrawing();
    }

    printf("[DEBUG] Loop principal encerrado.\n");

    /*
     * Liberação de Recursos
     */
    wangSolverDestroy(&solver);
    free(tileSet);

    if (executionCreated) {
        for (int i = 0; i < machine.tapeCount; i++) {
            tmTapeDestroy(&tapes[i]);
        }
        free(tapes);
        tmDestroy(&machine);
    }

    printf("[DEBUG] Fechando janela...\n");
    CloseWindow();
    printf("[DEBUG] Programa encerrado normalmente.\n");

    return 0;
}