#include "raylib.h"

#include "renderer.h"
#include "tile.h"
#include "tm.h"

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
        "Solucionador de Ladrilhos de Wang"
    );

    printf("[DEBUG] Janela inicializada.\n");

    SetTargetFPS(60);

    TuringMachine machine;

    printf("[DEBUG] Carregando maquina de Turing: machine.txt\n");

    if (!tmLoad(
        "machine.txt",
        &machine
    ))
    {
        printf("[ERRO] Falha ao carregar machine.txt.\n");
        printf("[DEBUG] Fechando janela.\n");

        CloseWindow();

        return 1;
    }

    printf("[DEBUG] Maquina carregada com sucesso.\n");
    printf("[DEBUG] Estados: %d\n", machine.stateCount);
    printf("[DEBUG] Fitas: %d\n", machine.tapeCount);
    printf("[DEBUG] Transicoes: %d\n", machine.transitionCount);
    printf("[DEBUG] Estado inicial: %d\n", machine.initialState);
    printf("[DEBUG] Fita inicial: %d\n", machine.initialTape);
    printf("[DEBUG] Estado de aceitacao: %d\n", machine.acceptState);
    printf("[DEBUG] Estado de rejeicao: %d\n", machine.rejectState);

    /*
     * Cria todas as fitas da máquina.
     */
    printf("[DEBUG] Criando vetor de fitas...\n");

    TMTape *tapes = calloc(
        (size_t)machine.tapeCount,
        sizeof(TMTape)
    );

    if (tapes == NULL)
    {
        printf("[ERRO] Falha ao alocar memoria para as fitas.\n");

        tmDestroy(&machine);

        printf("[DEBUG] Fechando janela.\n");

        CloseWindow();

        return 1;
    }

    printf("[DEBUG] Vetor de fitas criado.\n");

    /*
     * Tamanho inicial de cada fita.
     *
     * Por enquanto todas possuem o mesmo
     * tamanho. Posteriormente podemos
     * permitir tamanhos diferentes.
     */
    const size_t tapeSize = 100;

    printf(
        "[DEBUG] Tamanho de cada fita: %zu\n",
        tapeSize
    );

    bool tapesCreated = true;

    for (
        int i = 0;
        i < machine.tapeCount;
        i++
    )
    {
        printf(
            "[DEBUG] Criando fita %d...\n",
            i
        );

        if (!tmTapeCreate(
            &tapes[i],
            tapeSize
        ))
        {
            printf(
                "[ERRO] Falha ao criar fita %d.\n",
                i
            );

            tapesCreated = false;

            break;
        }

        printf(
            "[DEBUG] Fita %d criada com sucesso.\n",
            i
        );
    }

    if (!tapesCreated)
    {
        printf("[ERRO] Nao foi possivel criar todas as fitas.\n");
        printf("[DEBUG] Liberando fitas ja criadas.\n");

        for (
            int i = 0;
            i < machine.tapeCount;
            i++
        )
        {
            tmTapeDestroy(
                &tapes[i]
            );
        }

        free(tapes);

        tmDestroy(&machine);

        printf("[DEBUG] Fechando janela.\n");

        CloseWindow();

        return 1;
    }

    printf("[DEBUG] Todas as fitas foram criadas.\n");

    // Init test

    /*
     * Dados iniciais para teste.
     *
     * A fita começa assim:
     *
     * 0 0 0 _ _ _ _ ...
     * ^
     * cabeca
     */
    tapes[0].cells[0] = '0';
    tapes[0].cells[1] = '0';
    tapes[0].cells[2] = '0';
    tapes[0].cells[3] = '0';

    printf("[DEBUG] Dados iniciais colocados na fita 0.\n");

    printf("[DEBUG] Fita 0: ");

    for (
        size_t i = 0;
        i < 10;
        i++
    )
    {
        printf(
            "%c ",
            tapes[0].cells[i]
        );
    }

    printf("\n");

    // end test

    TMExecution execution;

    printf("[DEBUG] Criando execucao da maquina...\n");

    if (!tmExecutionCreate(
        &execution,
        &machine,
        tapes
    ))
    {
        printf("[ERRO] Falha ao criar execucao da maquina.\n");

        for (
            int i = 0;
            i < machine.tapeCount;
            i++
        )
        {
            tmTapeDestroy(
                &tapes[i]
            );
        }

        free(tapes);

        tmDestroy(&machine);

        printf("[DEBUG] Fechando janela.\n");

        CloseWindow();

        return 1;
    }

    printf("[DEBUG] Execucao criada com sucesso.\n");
    printf(
        "[DEBUG] Estado atual: %d\n",
        execution.currentState
    );
    printf(
        "[DEBUG] Fita ativa: %d\n",
        execution.activeTape
    );

    Tile tile = tileCreate(
        1,
        TILE_COLOR_RED,
        TILE_COLOR_BLUE,
        TILE_COLOR_GREEN,
        TILE_COLOR_YELLOW
    );

    printf("[DEBUG] Ladrilho criado.\n");

    tilePrint(&tile);

    printf("[DEBUG] Entrando no loop principal.\n");

    while (!WindowShouldClose())
    {
        rendererHandleTMControls(
            &execution
        );

        BeginDrawing();

        ClearBackground(
            RAYWHITE
        );

        DrawText(
            "Solucionador de Ladrilhos de Wang",
            20,
            20,
            30,
            BLACK
        );

        /*
         * Ladrilho.
         */
        rendererDrawTile(
            &tile,
            100,
            100,
            150
        );

        /*
         * Fita abaixo dos ladrilhos.
         *
         * O x representa o centro da fita.
         */
        rendererDrawTM(
            &execution,
            screenWidth / 2,
            400,
            40
        );

        DrawText(
            "SPACE - Proximo passo",
            20,
            620,
            20,
            BLACK
        );

        DrawText(
            "BACKSPACE - Passo anterior",
            20,
            650,
            20,
            BLACK
        );

        DrawText(
            "R - Reiniciar",
            20,
            680,
            20,
            BLACK
        );

        EndDrawing();
    }

    printf("[DEBUG] Loop principal encerrado.\n");

    if (WindowShouldClose())
    {
        printf(
            "[DEBUG] WindowShouldClose() retornou true.\n"
        );
    }

    /*
     * Libera todas as fitas.
     */
    printf("[DEBUG] Liberando fitas...\n");

    for (
        int i = 0;
        i < machine.tapeCount;
        i++
    )
    {
        printf(
            "[DEBUG] Liberando fita %d...\n",
            i
        );

        tmTapeDestroy(
            &tapes[i]
        );
    }

    free(tapes);

    printf("[DEBUG] Fitas liberadas.\n");

    printf("[DEBUG] Destruindo maquina...\n");

    tmDestroy(
        &machine
    );

    printf("[DEBUG] Maquina destruida.\n");

    printf("[DEBUG] Fechando janela...\n");

    CloseWindow();

    printf("[DEBUG] Programa encerrado normalmente.\n");

    return 0;
}