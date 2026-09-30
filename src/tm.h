#ifndef TM_H
#define TM_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    TM_MOVE_LEFT,
    TM_MOVE_RIGHT,
    TM_MOVE_UP,
    TM_MOVE_DOWN,
    TM_MOVE_STAY
} TMMove;

typedef struct {
    int currentState;

    char readSymbol;

    int nextState;
    char writeSymbol;

    TMMove move;
} TMTransition;

typedef struct {
    char **cells;

    size_t rows;
    size_t columns;

    size_t headRow;
    size_t headColumn;
} TMTape;

typedef struct {
    int stateCount;
    int transitionCount;

    TMTransition *transitions;

    int initialState;
    int acceptState;
    int rejectState;
} TuringMachine;

typedef struct {
    TuringMachine *machine;

    TMTape *tape;

    int currentState;

    bool halted;
    bool accepted;

    size_t stepCount;
} TMExecution;

bool tmLoad(
    const char *filename,
    TuringMachine *machine
);

void tmDestroy(
    TuringMachine *machine
);

bool tmTapeCreate(
    TMTape *tape,
    size_t rows,
    size_t columns
);

void tmTapeDestroy(
    TMTape *tape
);

bool tmExecutionCreate(
    TMExecution *execution,
    TuringMachine *machine,
    TMTape *tape
);

void tmExecutionReset(
    TMExecution *execution
);

bool tmStep(
    TMExecution *execution
);

bool tmStepBack(
    TMExecution *execution
);

bool tmIsHalted(
    const TMExecution *execution
);

#endif