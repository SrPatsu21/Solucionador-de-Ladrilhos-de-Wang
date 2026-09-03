# Solucionador de Ladrilhos de Wang

Implementação em **C** de um solucionador de Ladrilhos de Wang, com visualização gráfica da grade e preparação para a redução de uma **Máquina de Turing (MT)** para um conjunto de ladrilhos.

## 1. Objetivo do projeto

1. Representar cada ladrilho como:

```c
typedef struct {
    int id;
    Color top;
    Color right;
    Color bottom;
    Color left;
} Tile;
```

2. Resolver uma grade `N × M` usando **backtracking/DFS**.
3. Verificar compatibilidade com o ladrilho de cima e o ladrilho da esquerda.
4. Fazer backtracking quando uma escolha levar a um beco sem saída.
5. Gerar um conjunto de ladrilhos equivalente a uma especificação de **Máquina de Turing**.
6. Visualizar a grade de forma gráfica, usando cores diferentes para representar símbolos/estados nas bordas.
7. Permitir, posteriormente, importar uma MT e visualizar os ladrilhos gerados.

---

## 2. Biblioteca gráfica escolhida: raylib

A raylib é escrita em C, possui uma API pequena e direta e já oferece as operações necessárias para criar uma janela, desenhar retângulos/linhas/texto e tratar teclado e mouse. A própria documentação a descreve como uma biblioteca simples e fácil de usar. citeturn0search3turn0search1

Ela é especialmente adequada aqui porque não precisamos de uma interface gráfica complexa: a maior parte da aplicação será um algoritmo em C e a interface precisa apenas:

- abrir uma janela;
- desenhar a grade;
- desenhar cada ladrilho;
- colorir as quatro bordas;
- mostrar IDs, estados e símbolos;
- receber comandos do teclado/mouse;
- futuramente oferecer botões para carregar uma MT e iniciar a execução.

---

# 3. Instalação no Arch Linux

## 3.0 Pule o passo 3.1

## 3.1 Atualizar o sistema

Primeiro, atualize os pacotes:

```bash
sudo pacman -Syu
```

## 3.2 Instalar compilador e ferramentas

Instale o conjunto básico de desenvolvimento:

```bash
sudo pacman -S --needed base-devel
```

Isso fornece as ferramentas essenciais para compilar programas C.

Também recomendamos instalar o Git:

```bash
sudo pacman -S --needed git
```

## 3.3 Instalar raylib

A raylib está disponível diretamente nos repositórios oficiais do Arch Linux:

```bash
sudo pacman -S --needed raylib
```

Depois disso, o sistema deverá disponibilizar o header:

```text
/usr/include/raylib.h
```

e a biblioteca compartilhada:

```text
/usr/lib/libraylib.so
```

---

# 4. Verificar a instalação

Execute:

```bash
pacman -Q raylib
```

Também é possível verificar o header:

```bash
ls /usr/include/raylib.h
```

E a biblioteca:

```bash
ls /usr/lib/libraylib.so
```

Se esses arquivos existirem, a raylib está instalada.

---

# 5. Estrutura inicial do projeto

Conforme o projeto crescer, recomendamos evoluir para:

```text
.
├── README.md
├── Makefile
├── src/
│   ├── main.c
│   ├── tile.c
│   ├── tile.h
│   ├── solver.c
│   ├── solver.h
│   ├── tm.c
│   ├── tm.h
│   ├── reduction.c
│   ├── reduction.h
│   ├── renderer.c
│   └── renderer.h
└── assets/
```

A separação é importante porque o algoritmo de resolução não deve depender da interface gráfica.

---

# 6. Primeiro programa gráfico para teste

No arquivo `src/main.c`:

```c
#include "raylib.h"

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Solucionador de Ladrilhos de Wang");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(
            "Solucionador de Ladrilhos de Wang",
            20,
            20,
            30,
            BLACK
        );

        DrawText(
            "Raylib funcionando!",
            20,
            70,
            20,
            DARKGRAY
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
```

Compile:

```bash
cc -std=c11 -Wall -Wextra -pedantic src/main.c -o wang_solver -lraylib -lm
```

Execute:

```bash
./wang_solver
```

Se uma janela aparecer, a instalação está funcionando.

---

# 7. Teste inicial desenhando um ladrilho

Antes de implementar o backtracking, vale testar a representação gráfica.

Um ladrilho pode ser desenhado como um quadrado com quatro bordas coloridas:

```text
          TOP
       ─────────
      │         │
 LEFT │   ID    │ RIGHT
      │         │
       ─────────
         BOTTOM
```

---

# 8. Estrutura dos dados

```c
typedef enum {
    COLOR_EMPTY,
    COLOR_0,
    COLOR_1,
    COLOR_STATE_A,
    COLOR_STATE_B
} TileColor;
```

E:

```c
typedef struct {
    int id;

    TileColor top;
    TileColor right;
    TileColor bottom;
    TileColor left;
} Tile;
```

Assim, o solver não precisa conhecer RGB.

A conversão para uma cor da raylib fica somente no renderer:

```c
Color tileColorToRaylib(TileColor color);
```

Isso mantém o algoritmo independente da biblioteca gráfica.

---

# 9. Grade

A grade pode ser representada inicialmente como um vetor ou, para uma implementação mais simples quando `N` e `M` são conhecidos em tempo de execução:

```c
Tile *grid;
```

com acesso:

```c
grid[r * M + c]
```

Para marcar uma posição vazia:

```c
Tile *grid;
```

e usar `NULL` em cada posição.

---

# 10. Backtracking

O núcleo do programa deve ficar independente da raylib.

A ideia é:

```text
resolver(r, c)
    |
    +-- acabou a grade?
    |      |
    |      +-- SIM -> solução encontrada
    |
    +-- para cada ladrilho
           |
           +-- encaixa?
                  |
                  +-- NÃO -> próximo ladrilho
                  |
                  +-- SIM
                       |
                       +-- colocar ladrilho
                       |
                       +-- resolver próxima posição
                       |
                       +-- encontrou solução?
                       |       |
                       |       +-- SIM -> retornar sucesso
                       |
                       +-- remover ladrilho
                       |
                       +-- tentar próximo
```

A função deve ser aproximadamente:

```c
bool solveWang(
    Tile **grid,
    const Tile *tiles,
    int tileCount,
    int r,
    int c,
    int N,
    int M
);
```

E a verificação:

```c
bool canPlace(
    Tile **grid,
    Tile tile,
    int r,
    int c,
    int N,
    int M
);
```

---

# 11. Regra de encaixe

Para a posição `(r, c)`:

### Vizinho de cima

Se `r > 0`:

```c
tile.top == grid[r - 1][c]->bottom
```

### Vizinho da esquerda

Se `c > 0`:

```c
tile.left == grid[r][c - 1]->right
```

Portanto:

```c
bool canPlace(...)
{
    if (r > 0 &&
        tile.top != grid[r - 1][c]->bottom)
        return false;

    if (c > 0 &&
        tile.left != grid[r][c - 1]->right)
        return false;

    return true;
}
```

---

# 12. Ordem das posições

A grade deve ser preenchida linha por linha:

```text
(0,0) -> (0,1) -> (0,2) -> ...

(1,0) -> (1,1) -> (1,2) -> ...

(2,0) -> ...
```

Isso é importante porque, quando estamos preenchendo `(r,c)`, os únicos vizinhos que já foram preenchidos são:

```text
       cima
        ↓
    ┌───────┐
esq │  atual│
    └───────┘
```

Logo, só precisamos verificar **cima** e **esquerda** durante a construção.

---

# 13. Visualização do backtracking

Uma funcionalidade muito interessante para a apresentação é mostrar o algoritmo funcionando passo a passo.

Por exemplo:

```text
Tentativa 1
┌───┬───┬───┐
│ 2 │   │   │
├───┼───┼───┤
│   │   │   │
└───┴───┴───┘

Tentativa 2
┌───┬───┬───┐
│ 2 │ 7 │   │
├───┼───┼───┤
│   │   │   │
└───┴───┴───┘

Beco sem saída!

Backtrack
        ↓

┌───┬───┬───┐
│ 2 │ 3 │   │
├───┼───┼───┤
│   │   │   │
└───┴───┴───┘
```

Isso deixa o funcionamento do algoritmo muito mais fácil de demonstrar ao professor.

Uma opção é executar uma tentativa por frame, ou inserir um pequeno atraso entre passos.

---

# 14. Redução da Máquina de Turing

A parte mais importante conceitualmente será separar:

```text
Máquina de Turing
       |
       v
Gerador de ladrilhos
       |
       v
Conjunto de Wang
       |
       v
Solver de Wang
       |
       v
Grade
       |
       v
Visualização
```

A grade de Wang deverá representar a evolução da configuração da MT.

Conceitualmente:

```text
tempo 0    configuração inicial
   ↓
tempo 1    configuração após uma transição
   ↓
tempo 2    configuração após outra transição
   ↓
tempo 3    ...
```

Ou seja, cada linha da grade representa uma configuração da fita em um instante da computação.

---

# 15. Estrutura para a Máquina de Turing

Uma possível estrutura:

```c
typedef struct {
    int id;
    int currentState;
    char readSymbol;
    char writeSymbol;
    char move;
    int nextState;
} TMTransition;
```

E:

```c
typedef struct {
    int stateCount;
    int symbolCount;
    int transitionCount;

    TMTransition *transitions;

    int initialState;
    int acceptState;
    int rejectState;
} TuringMachine;
```

---

# 16. Formato de importação


```text
states: 3
symbols: 0,1,_
initial: q0
accept: q2
reject: q3

q0 0 -> q0 1 R
q0 1 -> q1 0 L
q1 0 -> q2 1 R
```

Depois podemos implementar:

```c
bool loadTuringMachine(
    const char *filename,
    TuringMachine *tm
);
```

O fluxo será:

```text
arquivo.txt
    ↓
parser
    ↓
TuringMachine
    ↓
gerador de Wang
    ↓
Tile[]
```

---

# 17. Interface gráfica planejada

A primeira versão pode ter somente:

```text
┌─────────────────────────────────────────────────────┐
│ Solucionador de Ladrilhos de Wang                   │
├─────────────────────────────────────────────────────┤
│                                                     │
│        ┌───┬───┬───┬───┐                            │
│        │   │   │   │   │                            │
│        ├───┼───┼───┼───┤       CONTROLES            │
│        │   │   │   │   │                            │
│        ├───┼───┼───┼───┤       [ Resolver ]         │
│        │   │   │   │   │                            │
│        └───┴───┴───┴───┘       [ Passo ]            │
│                                                     │
│                                  [ Limpar ]          │
│                                                     │
│ Status: buscando...                                  │
└─────────────────────────────────────────────────────┘
```

Depois:

```text
[ Importar MT ]
[ Gerar Ladrilhos ]
[ Resolver ]
[ Passo a passo ]
[ Reset ]
```

---

# 18. Arquitetura recomendada

A divisão sera:

```text
src/
├── main.c
│
├── tile.h
├── tile.c
│   └── estrutura dos ladrilhos
│
├── solver.h
├── solver.c
│   └── backtracking
│
├── tm.h
├── tm.c
│   └── Máquina de Turing
│
├── reduction.h
├── reduction.c
│   └── MT -> Ladrilhos de Wang
│
├── renderer.h
└── renderer.c
    └── raylib
```

A regra principal é:

> `solver.c` não deve precisar incluir `raylib.h`.

Assim, podemos testar o algoritmo pelo terminal sem abrir uma janela.

---

# 19. Makefile

Depois do primeiro protótipo, recomendamos adicionar um `Makefile`.

Exemplo:

```make
CC = cc

CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2
LDFLAGS = -lraylib -lm

TARGET = wang_solver

SRC = \
    src/main.c \
    src/tile.c \
    src/solver.c \
    src/tm.c \
    src/reduction.c \
    src/renderer.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
```

Compilar:

```bash
make
```

Executar:

```bash
make run
```

Limpar:

```bash
make clean
```

---

# 20. Desenvolvimento em etapas

### Etapa 1 — Ambiente

- [x] C
- [x] compilador
- [x] raylib
- [x] janela gráfica

### Etapa 2 — Ladrilhos

- [ ] `Tile`
- [ ] cores
- [ ] vetor de ladrilhos
- [ ] desenho de um ladrilho
- [ ] desenho da grade

### Etapa 3 — Solver

- [ ] `canPlace`
- [ ] DFS
- [ ] backtracking
- [ ] detecção de solução
- [ ] detecção de impossibilidade

### Etapa 4 — Visualização

- [ ] mostrar tentativa atual
- [ ] mostrar backtracking
- [ ] botão "Resolver"
- [ ] botão "Passo"
- [ ] contador de tentativas

### Etapa 5 — Máquina de Turing

- [ ] estrutura da MT
- [ ] parser
- [ ] representação da fita
- [ ] transições
- [ ] gerador MT → Wang

### Etapa 6 — Integração

```text
Importar MT
     ↓
Validar MT
     ↓
Gerar ladrilhos
     ↓
Mostrar ladrilhos
     ↓
Construir grade
     ↓
Executar solver
     ↓
Mostrar computação
```

---

# 21. Referências

- raylib: biblioteca gráfica utilizada no projeto. citeturn0search3
- raylib Cheat Sheet 6.0: referência rápida das funções disponíveis. citeturn0search34
- Pacote raylib no Arch Linux: instalação e arquivos fornecidos pelo pacote. citeturn0search0
- ArchWiki — SDL: referência para a situação atual do SDL no Arch Linux. citeturn0search12
