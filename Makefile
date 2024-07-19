CC = gcc
CFLAGS = -std=c89 -Wall -Wextra
LIBS = `pkg-config sdl3 glew --cflags --libs` -framework OpenGL
INCLUDE = -Iinclude
TARGET = build
SRCS=$(wildcard src/**.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) 

.PHONY: all clean
