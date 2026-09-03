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
