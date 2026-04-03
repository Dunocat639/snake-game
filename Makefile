# Variables de compilador i flags
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++23 -g
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Nom de l'executable
TARGET = snake

# Troba automaticament tots els .c dins src/
SRC = $(wildcard src/*.cpp)

# Genera els .o corresponents (src/foo.c -> build/foo.o)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

# Perdefinir directoris d'output amb més claritat
BUILD_DIR = build
SRC_DIR = src

# Regla principal
all: $(TARGET)

# Enllaça tots els .o per crear l'executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compila cada .c a .o (inclou headers de src/)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I $(SRC_DIR) -c $< -o $@

# Neteu tots els fitxers temporals i directoris
clean:
	rm -rf $(TARGET) $(BUILD_DIR)

# Compila i executa directament
run: all
	./$(TARGET)