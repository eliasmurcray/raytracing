CC = gcc
CFLAGS = -std=c89 -Wall -Wextra
LIBS = `pkg-config sdl3 glew --cflags --libs` -framework OpenGL
TARGET = build/main
SRCS=$(wildcard src/**.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	mkdir -p $(dir $(TARGET))
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf $(TARGET) 

.PHONY: all clean
