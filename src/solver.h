#ifndef SOLVER_H
#define SOLVER_H

#include "tile.h"
#include <stdbool.h>
#include <stddef.h>

/*
 * Representa o tabuleiro (grade n x n) para os Ladrilhos de Wang.
 */
typedef struct {
    int width;
    int height;
    Tile **cells; // Matriz linearizada de ponteiros para os ladrilhos posicionados
} WangBoard;

/*
 * Estrutura do Solucionador por Backtracking
 */
typedef struct {
    int width;
    int height;
    const Tile *tileSet;
    size_t tileCount;
    WangBoard board;
    
    int currentIndex;   // Célula atual sendo processada no grid
    int *tileChoices;   // Armazena o índice do próximo ladrilho a testar em cada célula (pilha de estados)
    bool solved;        // Indica se encontrou uma solução completa
    bool failed;        // Indica se esgotou todas as possibilidades (sem solução)
} WangSolver;

/* Funções do Solver */
WangSolver wangSolverCreate(int width, int height, const Tile *tileSet, size_t tileCount);
void wangSolverDestroy(WangSolver *solver);
bool wangSolverStep(WangSolver *solver);
void wangSolverReset(WangSolver *solver);

#endif