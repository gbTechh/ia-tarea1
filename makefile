# Makefile para proyecto SDL2 y GLAD en C++ (multiplataforma)

# Compilador
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -g -Wall -Wextra -O0 -DDEBUG -Iinclude -Iinclude/SDL2 -Iinclude/glad -Isrc/Enginee
CFLAGS = -g -Wall -Iinclude -Iinclude/SDL2 -Iinclude/glad

# Detectar sistema operativo
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CXXFLAGS += $(shell pkg-config --cflags sdl2)
    CFLAGS += $(shell pkg-config --cflags sdl2)
    LDFLAGS = $(shell pkg-config --libs sdl2) -lGL
else ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llib -lSDL2 -lopengl32
endif

# Directorios
SRC_DIR = src
ENGINE_DIR = $(SRC_DIR)/Enginee
GLAD_DIR = $(SRC_DIR)/glad
CLIENT_DIR = $(SRC_DIR)/Client
BIN_DIR = bin
INC_DIR = include

# Archivos fuente
CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(ENGINE_DIR)/*.cpp) $(wildcard $(CLIENT_DIR)/*.cpp)
C_SRCS = $(wildcard $(GLAD_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(CPP_SRCS)) $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(C_SRCS))

# Nombre del ejecutable
ifeq ($(OS),Windows_NT)
    TARGET = $(BIN_DIR)/sdl_app.exe
else
    TARGET = $(BIN_DIR)/sdl_app
endif

# Asegura que el directorio bin existe
$(shell mkdir -p $(BIN_DIR) $(BIN_DIR)/Enginee $(BIN_DIR)/glad $(BIN_DIR)/Client)

# Regla principal
all: $(TARGET)

# Compilar el programa completo
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compilar .cpp a .o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar .c a .o (para glad.c)
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar todo
clean:
	rm -rf $(BIN_DIR)

# Ejecutar con gdb para debugging
debug: $(TARGET)
	gdb -ex run --args ./$(TARGET)

# Ejecutar con valgrind para detectar memory leaks
valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Ejecutar normalmente
run: $(TARGET)
	./$(TARGET)

# Mostrar información del sistema
info:
	@echo "Sistema: $(UNAME_S)"
	@echo "GLM instalado:"
	@pkg-config --exists glm && echo "Sí" || echo "No"
	@echo "Archivos CPP encontrados:"
	@echo $(CPP_SRCS)
	@echo "Objetos a generar:"
	@echo $(OBJS)

.PHONY: all clean run debug valgrind info