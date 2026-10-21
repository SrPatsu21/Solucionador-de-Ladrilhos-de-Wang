#ifndef SOLVER_H
#define SOLVER_H

#include "tile.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    int width;
    int height;
    Tile **cells; // Aponta diretamente para os ladrilhos no tileSet
} WangBoard;

/*
 * Estrutura para o snapshot do histórico completo
 */
typedef struct {
    int currentIndex;
    bool solved;
    bool failed;
    int *tileChoices;
    int *boardTileIndices; // Índices dos ladrilhos na grade (-1 se vazio)
} SolverHistoryEntry;

typedef struct {
    int width;
    int height;
    Tile *tileSet;
    size_t tileCount;
    WangBoard board;
    
    int currentIndex;   
    int *tileChoices;   
    bool solved;        
    bool failed;

    // Histórico de estados
    SolverHistoryEntry *history;
    size_t historyCount;
    size_t historyCapacity;
} WangSolver;

WangSolver wangSolverCreate(int width, int height, Tile *tileSet, size_t tileCount);
void wangSolverDestroy(WangSolver *solver);
bool wangSolverStep(WangSolver *solver);
bool wangSolverStepBack(WangSolver *solver);
void wangSolverReset(WangSolver *solver);

#endif