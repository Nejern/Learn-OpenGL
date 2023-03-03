# Компилятор
CXX := clang++

# Флаги сборки
CXX_FLAGS := -Wall -Wextra -Wfatal-errors -Wpedantic -Wconversion -Wshadow
# Флаги оптимизации
O_FLAGS := -O0
# Флаги зависимостей
L_FLAGS := -ldl -lglfw -lassimp

TARGET := prog
SRC := src
SOURCES := $(SRC)/*

$(TARGET): $(SOURCES) include/*
	@echo "Building..."; $(CXX) $(O_FLAGS) $(SOURCES) -o $@ $(CXX_FLAGS) $(L_FLAGS) -Iinclude/imgui
