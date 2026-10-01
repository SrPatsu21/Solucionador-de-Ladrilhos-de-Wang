#include "tm.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool tmParseState(
    const char *text,
    int *state
)
{
    if (
        text == NULL ||
        state == NULL
    )
    {
        return false;
    }

    if (text[0] != 'q')
    {
        return false;
    }

    if (text[1] == '\0')
    {
        return false;
    }

    char *end = NULL;

    long value = strtol(
        text + 1,
        &end,
        10
    );

    if (
        end == text + 1 ||
        *end != '\0' ||
        value < 0 ||
        value > INT_MAX
    )
    {
        return false;
    }

    *state = (int)value;

    return true;
}

static bool tmParseMove(
    char value,
    TMMove *move
)
{
    if (move == NULL)
    {
        return false;
    }

    switch (value)
    {
        case 'L':
            *move = TM_MOVE_LEFT;
            return true;

        case 'R':
            *move = TM_MOVE_RIGHT;
            return true;

        case 'S':
            *move = TM_MOVE_STAY;
            return true;

        default:
            return false;
    }
}

static bool tmIsBlankLine(
    const char *line
)
{
    if (line == NULL)
    {
        return true;
    }

    for (const char *p = line; *p != '\0'; p++)
    {
        if (
            *p != ' ' &&
            *p != '\t' &&
            *p != '\n' &&
            *p != '\r'
        )
        {
            return false;
        }
    }

    return true;
}

static bool tmIsComment(
    const char *line
)
{
    if (line == NULL)
    {
        return false;
    }

    while (
        *line == ' ' ||
        *line == '\t'
    )
    {
        line++;
    }

    return *line == '#';
}

static TMTransition *tmFindTransition(
    TuringMachine *machine,
    int currentState,
    char readSymbol
)
{
    if (machine == NULL)
    {
        return NULL;
    }

    for (
        int i = 0;
        i < machine->transitionCount;
        i++
    )
    {
        TMTransition *transition =
            &machine->transitions[i];

        if (
            transition->currentState == currentState &&
            transition->action.readSymbol == readSymbol
        )
        {
            return transition;
        }
    }

    return NULL;
}

bool tmLoad(
    const char *filename,
    TuringMachine *machine
)
{
    if (
        filename == NULL ||
        machine == NULL
    )
    {
        return false;
    }

    FILE *file = fopen(
        filename,
        "r"
    );

    if (file == NULL)
    {
        return false;
    }

    machine->stateCount = 0;
    machine->tapeCount = 0;
    machine->transitionCount = 0;

    machine->transitions = NULL;

    machine->initialState = 0;
    machine->initialTape = 0;

    machine->acceptState = 0;
    machine->rejectState = 0;

    char line[256];

    /*
     * First pass:
     * read machine configuration.
     */
    while (
        fgets(
            line,
            sizeof(line),
            file
        ) != NULL
    )
    {
        if (
            tmIsBlankLine(line) ||
            tmIsComment(line)
        )
        {
            continue;
        }

        if (
            strncmp(
                line,
                "states:",
                7
            ) == 0
        )
        {
            if (
                sscanf(
                    line,
                    "states: %d",
                    &machine->stateCount
                ) != 1
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
        else if (
            strncmp(
                line,
                "tapes:",
                6
            ) == 0
        )
        {
            if (
                sscanf(
                    line,
                    "tapes: %d",
                    &machine->tapeCount
                ) != 1
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
        else if (
            strncmp(
                line,
                "initial:",
                8
            ) == 0
        )
        {
            char stateText[32];

            if (
                sscanf(
                    line,
                    "initial: %31s",
                    stateText
                ) != 1 ||
                !tmParseState(
                    stateText,
                    &machine->initialState
                )
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
        else if (
            strncmp(
                line,
                "initial-tape:",
                13
            ) == 0
        )
        {
            if (
                sscanf(
                    line,
                    "initial-tape: %d",
                    &machine->initialTape
                ) != 1
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
        else if (
            strncmp(
                line,
                "accept:",
                7
            ) == 0
        )
        {
            char stateText[32];

            if (
                sscanf(
                    line,
                    "accept: %31s",
                    stateText
                ) != 1 ||
                !tmParseState(
                    stateText,
                    &machine->acceptState
                )
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
        else if (
            strncmp(
                line,
                "reject:",
                7
            ) == 0
        )
        {
            char stateText[32];

            if (
                sscanf(
                    line,
                    "reject: %31s",
                    stateText
                ) != 1 ||
                !tmParseState(
                    stateText,
                    &machine->rejectState
                )
            )
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }
        }
    }

    if (
        machine->stateCount <= 0 ||
        machine->tapeCount <= 0 ||
        machine->initialState < 0 ||
        machine->initialState >= machine->stateCount ||
        machine->acceptState < 0 ||
        machine->acceptState >= machine->stateCount ||
        machine->rejectState < 0 ||
        machine->rejectState >= machine->stateCount ||
        machine->initialTape < 0 ||
        machine->initialTape >= machine->tapeCount
    )
    {
        fclose(file);
        tmDestroy(machine);
        return false;
    }

    /*
     * Second pass:
     * read transitions.
     */
    rewind(file);

    int transitionCapacity = 0;

    while (
        fgets(
            line,
            sizeof(line),
            file
        ) != NULL
    )
    {
        if (
            tmIsBlankLine(line) ||
            tmIsComment(line)
        )
        {
            continue;
        }

        char currentStateText[32];
        char nextStateText[32];

        char readSymbol;
        char writeSymbol;
        char moveSymbol;

        int nextTape;

        int result = sscanf(
            line,
            "%31s %c -> %31s %c %c %d",
            currentStateText,
            &readSymbol,
            nextStateText,
            &writeSymbol,
            &moveSymbol,
            &nextTape
        );

        if (result != 6)
        {
            /*
             * Configuration lines were already processed
             * in the first pass. They are not transitions.
             */
            continue;
        }

        int currentState;
        int nextState;

        if (
            !tmParseState(
                currentStateText,
                &currentState
            ) ||
            !tmParseState(
                nextStateText,
                &nextState
            )
        )
        {
            fclose(file);
            tmDestroy(machine);
            return false;
        }

        if (
            currentState < 0 ||
            currentState >= machine->stateCount ||
            nextState < 0 ||
            nextState >= machine->stateCount ||
            nextTape < 0 ||
            nextTape >= machine->tapeCount
        )
        {
            fclose(file);
            tmDestroy(machine);
            return false;
        }

        TMMove move;

        if (
            !tmParseMove(
                moveSymbol,
                &move
            )
        )
        {
            fclose(file);
            tmDestroy(machine);
            return false;
        }

        if (
            machine->transitionCount >=
            transitionCapacity
        )
        {
            transitionCapacity =
                transitionCapacity == 0
                    ? 8
                    : transitionCapacity * 2;

            TMTransition *transitions =
                realloc(
                    machine->transitions,
                    (size_t)transitionCapacity *
                    sizeof(TMTransition)
                );

            if (transitions == NULL)
            {
                fclose(file);
                tmDestroy(machine);
                return false;
            }

            machine->transitions =
                transitions;
        }

        TMTransition *transition =
            &machine->transitions[
                machine->transitionCount
            ];

        transition->currentState =
            currentState;

        transition->action.readSymbol =
            readSymbol;

        transition->action.writeSymbol =
            writeSymbol;

        transition->action.move =
            move;

        transition->action.nextTape =
            nextTape;

        transition->nextState =
            nextState;

        machine->transitionCount++;
    }

    fclose(file);

    return true;
}

void tmDestroy(
    TuringMachine *machine
)
{
    if (machine == NULL)
    {
        return;
    }

    free(
        machine->transitions
    );

    machine->transitions = NULL;

    machine->stateCount = 0;
    machine->tapeCount = 0;
    machine->transitionCount = 0;

    machine->initialState = 0;
    machine->initialTape = 0;

    machine->acceptState = 0;
    machine->rejectState = 0;
}

bool tmTapeCreate(
    TMTape *tape,
    size_t size
)
{
    if (
        tape == NULL ||
        size == 0
    )
    {
        return false;
    }

    tape->cells = malloc(
        size * sizeof(char)
    );

    if (tape->cells == NULL)
    {
        return false;
    }

    tape->size = size;
    tape->head = 0;

    for (
        size_t i = 0;
        i < size;
        i++
    )
    {
        tape->cells[i] = '_';
    }

    return true;
}

void tmTapeDestroy(
    TMTape *tape
)
{
    if (tape == NULL)
    {
        return;
    }

    free(
        tape->cells
    );

    tape->cells = NULL;
    tape->size = 0;
    tape->head = 0;
}

static bool tmHistoryPush(
    TMExecution *execution,
    TMHistoryEntry entry
)
{
    if (execution == NULL)
    {
        return false;
    }

    if (
        execution->historyCount >=
        execution->historyCapacity
    )
    {
        size_t newCapacity =
            execution->historyCapacity == 0
                ? 32
                : execution->historyCapacity * 2;

        TMHistoryEntry *history =
            realloc(
                execution->history,
                newCapacity *
                sizeof(TMHistoryEntry)
            );

        if (history == NULL)
        {
            return false;
        }

        execution->history =
            history;

        execution->historyCapacity =
            newCapacity;
    }

    execution->history[
        execution->historyCount
    ] = entry;

    execution->historyCount++;

    return true;
}

bool tmExecutionCreate(
    TMExecution *execution,
    TuringMachine *machine,
    TMTape *tapes
)
{
    if (
        execution == NULL ||
        machine == NULL ||
        tapes == NULL
    )
    {
        return false;
    }

    execution->machine = machine;
    execution->tapes = tapes;

    execution->currentState =
        machine->initialState;

    execution->activeTape =
        machine->initialTape;

    execution->halted = false;
    execution->accepted = false;

    execution->stepCount = 0;

    execution->history = NULL;
    execution->historyCount = 0;
    execution->historyCapacity = 0;

    return true;
}

void tmExecutionReset(
    TMExecution *execution
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

    execution->currentState =
        execution->machine->initialState;

    execution->activeTape =
        execution->machine->initialTape;

    execution->halted = false;
    execution->accepted = false;

    execution->stepCount = 0;

    execution->historyCount = 0;

    for (
        int i = 0;
        i < execution->machine->tapeCount;
        i++
    )
    {
        execution->tapes[i].head = 0;
    }
}

bool tmStep(
    TMExecution *execution
)
{
    if (
        execution == NULL ||
        execution->machine == NULL ||
        execution->tapes == NULL
    )
    {
        return false;
    }

    if (execution->halted)
    {
        return false;
    }

    if (
        execution->currentState ==
        execution->machine->acceptState
    )
    {
        execution->halted = true;
        execution->accepted = true;

        return false;
    }

    if (
        execution->currentState ==
        execution->machine->rejectState
    )
    {
        execution->halted = true;
        execution->accepted = false;

        return false;
    }

    int tapeIndex =
        execution->activeTape;

    TMTape *tape =
        &execution->tapes[tapeIndex];

    if (
        tape->cells == NULL ||
        tape->size == 0 ||
        tape->head >= tape->size
    )
    {
        execution->halted = true;
        execution->accepted = false;

        return false;
    }

    char readSymbol =
        tape->cells[tape->head];

    TMTransition *transition =
        tmFindTransition(
            execution->machine,
            execution->currentState,
            readSymbol
        );

    if (transition == NULL)
    {
        execution->halted = true;
        execution->accepted = false;

        return false;
    }

    TMHistoryEntry history;

    history.previousState =
        execution->currentState;

    history.previousTape =
        execution->activeTape;

    history.tapeIndex =
        tapeIndex;

    history.headPosition =
        tape->head;

    history.previousSymbol =
        tape->cells[tape->head];

    if (
        !tmHistoryPush(
            execution,
            history
        )
    )
    {
        return false;
    }

    tape->cells[tape->head] =
        transition->action.writeSymbol;

    switch (
        transition->action.move
    )
    {
        case TM_MOVE_LEFT:
            if (tape->head > 0)
            {
                tape->head--;
            }
            break;

        case TM_MOVE_RIGHT:
            if (
                tape->head + 1 <
                tape->size
            )
            {
                tape->head++;
            }
            break;

        case TM_MOVE_STAY:
            break;
    }

    execution->currentState =
        transition->nextState;

    execution->activeTape =
        transition->action.nextTape;

    execution->stepCount++;

    if (
        execution->currentState ==
        execution->machine->acceptState
    )
    {
        execution->halted = true;
        execution->accepted = true;
    }
    else if (
        execution->currentState ==
        execution->machine->rejectState
    )
    {
        execution->halted = true;
        execution->accepted = false;
    }

    return true;
}

bool tmStepBack(
    TMExecution *execution
)
{
    if (
        execution == NULL ||
        execution->machine == NULL ||
        execution->tapes == NULL
    )
    {
        return false;
    }

    if (execution->historyCount == 0)
    {
        return false;
    }

    TMHistoryEntry *history =
        &execution->history[
            execution->historyCount - 1
        ];

    if (
        history->tapeIndex < 0 ||
        history->tapeIndex >=
            execution->machine->tapeCount
    )
    {
        return false;
    }

    TMTape *tape =
        &execution->tapes[
            history->tapeIndex
        ];

    if (
        history->headPosition >=
        tape->size
    )
    {
        return false;
    }

    /*
     * Restore the symbol.
     */
    tape->cells[
        history->headPosition
    ] = history->previousSymbol;

    /*
     * Restore the head position.
     */
    tape->head =
        history->headPosition;

    /*
     * Restore machine state.
     */
    execution->currentState =
        history->previousState;

    /*
     * Restore active tape.
     */
    execution->activeTape =
        history->previousTape;

    if (execution->stepCount > 0)
    {
        execution->stepCount--;
    }

    execution->historyCount--;

    execution->halted = false;
    execution->accepted = false;

    return true;
}

bool tmIsHalted(
    const TMExecution *execution
)
{
    if (execution == NULL)
    {
        return true;
    }

    return execution->halted;
}