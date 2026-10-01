#ifndef TM_H
#define TM_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    TM_MOVE_LEFT,
    TM_MOVE_RIGHT,
    TM_MOVE_STAY
} TMMove;

typedef struct {
    char readSymbol;
    char writeSymbol;

    TMMove move;

    int nextTape;
} TMTapeAction;

typedef struct {
    int currentState;

    TMTapeAction action;

    int nextState;
} TMTransition;

typedef struct {
    char *cells;

    size_t size;
    size_t head;
} TMTape;

typedef struct {
    int stateCount;
    int tapeCount;
    int transitionCount;

    TMTransition *transitions;

    int initialState;
    int initialTape;

    int acceptState;
    int rejectState;
} TuringMachine;

typedef struct {
    int previousState;
    int previousTape;

    int tapeIndex;
    size_t headPosition;

    char previousSymbol;
} TMHistoryEntry;

typedef struct {
    TuringMachine *machine;

    TMTape *tapes;

    int currentState;
    int activeTape;

    bool halted;
    bool accepted;

    size_t stepCount;

    TMHistoryEntry *history;
    size_t historyCount;
    size_t historyCapacity;
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
    size_t size
);

void tmTapeDestroy(
    TMTape *tape
);

bool tmExecutionCreate(
    TMExecution *execution,
    TuringMachine *machine,
    TMTape *tapes
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