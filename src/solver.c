#include "solver.h"
#include <stdlib.h>
#include <stdio.h>

WangSolver wangSolverCreate(int width, int height, const Tile *tileSet, size_t tileCount)
{
    WangSolver solver;
    solver.width = width;
    solver.height = height;
    solver.tileSet = tileSet;
    solver.tileCount = tileCount;
    solver.currentIndex = 0;
    solver.solved = false;
    solver.failed = false;

    size_t totalCells = (size_t)(width * height);
    
    // Inicializa o tabuleiro
    solver.board.width = width;
    solver.board.height = height;
    solver.board.cells = malloc(totalCells * sizeof(Tile*));
    for (size_t i = 0; i < totalCells; i++) {
        solver.board.cells[i] = NULL;
    }

    // Inicializa o vetor de escolhas (pilha de backtracking)
    solver.tileChoices = malloc(totalCells * sizeof(int));
    for (size_t i = 0; i < totalCells; i++) {
        solver.tileChoices[i] = 0;
    }

    return solver;
}

void wangSolverDestroy(WangSolver *solver)
{
    if (solver == NULL) return;

    if (solver->board.cells != NULL) {
        for (int i = 0; i < solver->width * solver->height; i++) {
            free(solver->board.cells[i]);
        }
        free(solver->board.cells);
    }
    
    free(solver->tileChoices);
    solver->board.cells = NULL;
    solver->tileChoices = NULL;
}

/*
 * Valida se o ladrilho candidato cumpre as regras de vizinhança:
 * - Borda Norte deve casar com a borda Sul do vizinho superior.
 * - Borda Oeste deve casar com a borda Leste do vizinho esquerdo.
 */
static bool checkConstraints(const WangBoard *board, int x, int y, const Tile *tile)
{
    // Vizinho superior
    if (y > 0) {
        Tile *upper = board->cells[(y - 1) * board->width + x];
        if (upper != NULL && tile->top != upper->bottom) {
            return false;
        }
    }

    // Vizinho esquerdo
    if (x > 0) {
        Tile *left = board->cells[y * board->width + (x - 1)];
        if (left != NULL && tile->left != left->right) {
            return false;
        }
    }

    return true;
}

bool wangSolverStep(WangSolver *solver)
{
    if (solver == NULL || solver->solved || solver->failed) {
        return false;
    }

    int totalCells = solver->width * solver->height;

    if (solver->currentIndex >= totalCells) {
        solver->solved = true;
        return true;
    }

    int x = solver->currentIndex % solver->width;
    int y = solver->currentIndex / solver->width;

    // Tenta encaixar um ladrilho válido na célula atual
    while (solver->tileChoices[solver->currentIndex] < (int)solver->tileCount) {
        int tileIndex = solver->tileChoices[solver->currentIndex];
        const Tile *candidate = &solver->tileSet[tileIndex];

        // Prepara para a próxima tentativa futura (avança o índice de escolha)
        solver->tileChoices[solver->currentIndex]++;

        if (checkConstraints(&solver->board, x, y, candidate)) {
            // Aloca e posiciona o ladrilho na célula
            Tile *placed = malloc(sizeof(Tile));
            *placed = *candidate;
            solver->board.cells[y * solver->width + x] = placed;

            // Avança para a próxima célula
            solver->currentIndex++;
            if (solver->currentIndex >= totalCells) {
                solver->solved = true;
            }
            return true;
        }
    }

    // Se esgotou os ladrilhos sem sucesso, faz o Backtracking
    solver->tileChoices[solver->currentIndex] = 0; // Reseta escolhas da célula atual
    solver->currentIndex--; // Retorna à célula anterior

    if (solver->currentIndex < 0) {
        solver->failed = true;
        return false;
    }

    // Remove o ladrilho da célula anterior para testar novas opções lá
    int prevX = solver->currentIndex % solver->width;
    int prevY = solver->currentIndex / solver->width;
    free(solver->board.cells[prevY * solver->width + prevX]);
    solver->board.cells[prevY * solver->width + prevX] = NULL;

    return true;
}

void wangSolverReset(WangSolver *solver)
{
    if (solver == NULL) return;

    int totalCells = solver->width * solver->height;
    for (int i = 0; i < totalCells; i++) {
        free(solver->board.cells[i]);
        solver->board.cells[i] = NULL;
        solver->tileChoices[i] = 0;
    }

    solver->currentIndex = 0;
    solver->solved = false;
    solver->failed = false;
}