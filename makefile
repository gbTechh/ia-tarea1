# Makefile para proyecto SDL2 en C++ (multiplataforma)

# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Iinclude -Iinclude/SDL2 -Isrc/Enginee

# Detectar sistema operativo
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CXXFLAGS += $(shell pkg-config --cflags sdl2)
    LDFLAGS = $(shell pkg-config --libs sdl2)
else ifeq ($(OS),Windows_NT)
    LDFLAGS = -Llib -lSDL2
endif

# Directorios
SRC_DIR = src
ENGINE_DIR = $(SRC_DIR)/Enginee
CLIENT_DIR = $(SRC_DIR)/Client
BIN_DIR = bin
INC_DIR = include

# Archivos fuente
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(ENGINE_DIR)/*.cpp) $(wildcard $(CLIENT_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRCS))

# Nombre del ejecutable
ifeq ($(OS),Windows_NT)
    TARGET = $(BIN_DIR)/sdl_app.exe
else
    TARGET = $(BIN_DIR)/sdl_app
endif

# Asegura que el directorio bin existe
$(shell mkdir -p $(BIN_DIR) $(BIN_DIR)/Enginee)

# Regla principal
all: $(TARGET)

# Compilar el programa completo
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compilar .cpp a .o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar todo
clean:
	rm -f $(TARGET) $(BIN_DIR)/*.o $(BIN_DIR)/Enginee/*.o

# Ejecutar
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run