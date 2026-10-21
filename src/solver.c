#include "solver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

WangSolver wangSolverCreate(int width, int height, Tile *tileSet, size_t tileCount)
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
    
    solver.board.width = width;
    solver.board.height = height;
    solver.board.cells = malloc(totalCells * sizeof(const Tile*));
    for (size_t i = 0; i < totalCells; i++) {
        solver.board.cells[i] = NULL;
    }

    solver.tileChoices = malloc(totalCells * sizeof(int));
    for (size_t i = 0; i < totalCells; i++) {
        solver.tileChoices[i] = 0;
    }

    solver.history = NULL;
    solver.historyCount = 0;
    solver.historyCapacity = 0;

    return solver;
}

void wangSolverDestroy(WangSolver *solver)
{
    if (solver == NULL) return;

    free(solver->board.cells);
    free(solver->tileChoices);

    if (solver->history != NULL) {
        for (size_t i = 0; i < solver->historyCount; i++) {
            free(solver->history[i].tileChoices);
            free(solver->history[i].boardTileIndices);
        }
        free(solver->history);
    }
    
    solver->board.cells = NULL;
    solver->tileChoices = NULL;
    solver->history = NULL;
}

/*
 * Salva um snapshot completo do estado atual antes de avançar o passo
 */
static bool solverHistoryPush(WangSolver *solver)
{
    if (solver->historyCount >= solver->historyCapacity) {
        size_t newCapacity = solver->historyCapacity == 0 ? 64 : solver->historyCapacity * 2;
        SolverHistoryEntry *newHistory = realloc(solver->history, newCapacity * sizeof(SolverHistoryEntry));
        if (newHistory == NULL) return false;
        solver->history = newHistory;
        solver->historyCapacity = newCapacity;
    }

    int totalCells = solver->width * solver->height;
    SolverHistoryEntry entry;
    entry.currentIndex = solver->currentIndex;
    entry.solved = solver->solved;
    entry.failed = solver->failed;
    
    entry.tileChoices = malloc(totalCells * sizeof(int));
    entry.boardTileIndices = malloc(totalCells * sizeof(int));

    for (int i = 0; i < totalCells; i++) {
        entry.tileChoices[i] = solver->tileChoices[i];
        if (solver->board.cells[i] == NULL) {
            entry.boardTileIndices[i] = -1;
        } else {
            // Descobre o índice do ladrilho no tileSet via aritmética de ponteiros
            ptrdiff_t idx = solver->board.cells[i] - solver->tileSet;
            entry.boardTileIndices[i] = (int)idx;
        }
    }

    solver->history[solver->historyCount++] = entry;
    return true;
}

static bool checkConstraints(const WangBoard *board, int width, int x, int y, const Tile *tile)
{
    if (y > 0) {
        const Tile *upper = board->cells[(y - 1) * width + x];
        if (upper != NULL && tile->top != upper->bottom) return false;
    }
    if (x > 0) {
        const Tile *left = board->cells[y * width + (x - 1)];
        if (left != NULL && tile->left != left->right) return false;
    }
    return true;
}

bool wangSolverStep(WangSolver *solver)
{
    if (solver == NULL || solver->solved || solver->failed) {
        return false;
    }

    // Salva o estado atual no histórico antes de alterar qualquer coisa
    if (!solverHistoryPush(solver)) {
        return false;
    }

    int totalCells = solver->width * solver->height;

    if (solver->currentIndex >= totalCells) {
        solver->solved = true;
        return true;
    }

    int x = solver->currentIndex % solver->width;
    int y = solver->currentIndex / solver->width;

    while (solver->tileChoices[solver->currentIndex] < (int)solver->tileCount) {
        int tileIndex = solver->tileChoices[solver->currentIndex];
        const Tile *candidate = &solver->tileSet[tileIndex];

        solver->tileChoices[solver->currentIndex]++;

        if (checkConstraints(&solver->board, solver->width, x, y, candidate)) {
            // Posiciona apontando diretamente para o tileSet (sem alocação extra)
            solver->board.cells[y * solver->width + x] = candidate;

            solver->currentIndex++;
            if (solver->currentIndex >= totalCells) {
                solver->solved = true;
            }
            return true;
        }
    }

    // Backtracking automático caso esgote as opções
    solver->tileChoices[solver->currentIndex] = 0;
    solver->currentIndex--;

    if (solver->currentIndex < 0) {
        solver->failed = true;
        return true;
    }

    int prevX = solver->currentIndex % solver->width;
    int prevY = solver->currentIndex / solver->width;
    solver->board.cells[prevY * solver->width + prevX] = NULL;

    return true;
}

/*
 * Restaura o estado exato salvo no último snapshot do histórico
 */
bool wangSolverStepBack(WangSolver *solver)
{
    if (solver == NULL || solver->historyCount == 0) {
        return false;
    }

    // Pega o último estado salvo
    SolverHistoryEntry entry = solver->history[solver->historyCount - 1];
    solver->historyCount--;

    // Restaura as variáveis de controle
    solver->currentIndex = entry.currentIndex;
    solver->solved = entry.solved;
    solver->failed = entry.failed;

    int totalCells = solver->width * solver->height;
    for (int i = 0; i < totalCells; i++) {
        solver->tileChoices[i] = entry.tileChoices[i];
        int tileIdx = entry.boardTileIndices[i];
        if (tileIdx == -1) {
            solver->board.cells[i] = NULL;
        } else {
            solver->board.cells[i] = &solver->tileSet[tileIdx];
        }
    }

    // Libera a memória do snapshot restaurado
    free(entry.tileChoices);
    free(entry.boardTileIndices);

    return true;
}

void wangSolverReset(WangSolver *solver)
{
    if (solver == NULL) return;

    int totalCells = solver->width * solver->height;
    for (int i = 0; i < totalCells; i++) {
        solver->board.cells[i] = NULL;
        solver->tileChoices[i] = 0;
    }

    solver->currentIndex = 0;
    solver->solved = false;
    solver->failed = false;

    if (solver->history != NULL) {
        for (size_t i = 0; i < solver->historyCount; i++) {
            free(solver->history[i].tileChoices);
            free(solver->history[i].boardTileIndices);
        }
        solver->historyCount = 0;
    }
}