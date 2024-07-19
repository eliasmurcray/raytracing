#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

void buffer_data(unsigned char *data) {
  int i, j;
  for (i = 0; i < IMAGE_HEIGHT; i++) {
    for (j = 0; j < IMAGE_WIDTH; j++) {
      double r = (double)j / (IMAGE_HEIGHT - 1);
      double g = (double)i / (IMAGE_WIDTH - 1);
      double b = 0.0;

      int ir = (int)(255.999 * r);
      int ig = (int)(255.999 * g);
      int ib = (int)(255.999 * b);

      data[(j * IMAGE_WIDTH + i) * 3 + 0] = ir;
      data[(j * IMAGE_WIDTH + i) * 3 + 1] = ig;
      data[(j * IMAGE_WIDTH + i) * 3 + 2] = ib;
    }
  }
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL3: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow(NULL, IMAGE_WIDTH, IMAGE_HEIGHT, SDL_WINDOW_OPENGL);
  if (window == NULL) {
    SDL_Log("Failed to create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GLContext ctx = SDL_GL_CreateContext(window);
  if (ctx == NULL) {
    SDL_Log("Failed to create SDL_GLContext: %s\n", SDL_GetError());
    return 1;
  }
  glEnable(GL_TEXTURE_2D);
  
  unsigned char *data = (unsigned char*)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
  if (!data) {
    fprintf(stderr, "Failed to allocated memory for data.\n");
    SDL_GL_DestroyContext(ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_Event event;
  unsigned char running = 1;
  while(running) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_EVENT_QUIT:
          running = 0;
          break;
      }
    }

    buffer_data(data);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glClearColor(0, 0, 0.4, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f( 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f( 1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f,  1.0f);
    glEnd();

    SDL_GL_SwapWindow(window);

    SDL_DelayNS(1000);
  }
  
  free(data);
  SDL_GL_DestroyContext(ctx);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
