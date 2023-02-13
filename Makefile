CC := g++
TARGET := prog
SOURCES := Source.cpp Headers/*
LFLAGS := -lGL -lglfw -lGLEW

$(TARGET): $(SOURCES)
	@echo " Linking..."
	$(CC) Source.cpp -o $(TARGET) $(LFLAGS)
