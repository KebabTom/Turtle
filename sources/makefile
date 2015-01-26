CFLAGS = `sdl2-config --cflags` -O4 -Wall -pedantic -std=c99 -lm
TARGET = turtle
SOURCES =  $(TARGET).c parser.c interpreter.c
LIBS =  `sdl2-config --libs`
CC = gcc

all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS) 
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	$(TARGET) 
