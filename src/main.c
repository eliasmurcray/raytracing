#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

char* read_file(const char* path) {
  FILE* file = fopen(path, "r");
  if (!file) {
      fprintf(stderr, "Failed to open file: %s\n", path);
      exit(-1);
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* content = (char*)malloc(length + 1);
  if (!content) {
      fprintf(stderr, "Failed to allocate memory for file content\n");
      exit(1);
  }
  fread(content, 1, length, file);
  content[length] = '\0';
  fclose(file);
  return content;
}

GLuint compile_shader(GLenum type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char err[512];
    glGetShaderInfoLog(shader, 512, NULL, err);
    fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", err);
  }
  return shader;
}

GLuint create_program(const char* vert_path, const char *frag_path) {
  char *vert_src = read_file(vert_path);
  char *frag_src = read_file(frag_path);
  GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_src);
  GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_src);
  GLuint program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char err[512];
    glGetProgramInfoLog(program, 512, NULL, err);
    fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n%s\n", err);
  }
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);
  free(vert_src);
  free(frag_src);
  return program;
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL3: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      SDL_GL_DestroyContext(ctx);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return -1;
  }

  GLuint program = create_program("src/vertex_shader.glsl", "src/fragment_shader.glsl");
  float vertices[] = {
      -1.0f, -1.0f,   0.0f, 0.0f,
       1.0f, -1.0f,   1.0f, 0.0f,
       1.0f,  1.0f,   1.0f, 1.0f,
      -1.0f,  1.0f,   0.0f, 1.0f
  };

  unsigned int indices[] = {
      0, 1, 2,
      2, 3, 0
  };

  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

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
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    SDL_GL_SwapWindow(window);

    SDL_DelayNS(1000000);
  }


  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(program);
  SDL_GL_DestroyContext(ctx);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
