CC := clang++
TARGET := prog
SRC := src
SOURCES := src/*
LFLAGS := -ldl -lglfw

$(TARGET): $(SOURCES) include/*
	@echo " Linking..."; $(CC) $(SOURCES) -o $@ $(LFLAGS)
