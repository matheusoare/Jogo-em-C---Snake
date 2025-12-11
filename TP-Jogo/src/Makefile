# Nome do executável
TARGET = snake

# Compilador
CC = gcc

# Caminho da Raylib
RAYLIB_PATH = ./raylib/src

# Flags de Compilação (Padrão e Seguras)
# -Wall: Avisos
# -std=c99: Padrão C (compatível)
# -D_DEFAULT_SOURCE: Compatibilidade Linux
# -Wno-missing-braces: Limpa avisos chatos da raylib
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -I$(RAYLIB_PATH)

# Flags de Linkagem (Padrão Linux)
LDFLAGS = -L$(RAYLIB_PATH) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Arquivos
SRCS = main.c snake.c
OBJS = $(SRCS:.c=.o)

# Regras
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)