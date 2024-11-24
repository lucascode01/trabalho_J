# Nome do compilador
CC = clang

# Caminhos para os cabeçalhos e bibliotecas
INCLUDE_DIR = -Iinclude \
              -I/opt/homebrew/include \
              -I/opt/homebrew/Cellar/sdl2/2.30.9/include/SDL2 \
              -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2
LIB_DIR = -L/opt/homebrew/lib \
          -L/opt/homebrew/Cellar/sdl2/2.30.9/lib \
          -L/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib

# Flags para compilação e linkagem
CFLAGS = -Wall -Wextra -std=c99 $(INCLUDE_DIR)
LDFLAGS = $(LIB_DIR) -lSDL2 -lSDL2_ttf

# Diretórios
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Lista automática de arquivos fonte
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Nome do executável
BIN = $(BIN_DIR)/game

# Regra padrão
all: $(BIN)

# Regra para criar o executável
$(BIN): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Regra para compilar arquivos .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos compilados
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
