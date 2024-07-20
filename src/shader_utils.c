#include "shader_utils.h"

static char* freadstr(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) return NULL;
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buffer = (char *)malloc(len + 1);
  if (buffer) {
    fread(buffer, 1, len, file);
    buffer[len] = '\0';
  }
  fclose(file);
  return buffer;
}

static GLuint glshader(GLenum type, const char* src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
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

GLuint glprogram(const char* vpath, const char *fpath) {
  char *vsrc = freadstr(vpath);
  char *fsrc = freadstr(fpath);
  GLuint vs = glshader(GL_VERTEX_SHADER, vsrc);
  GLuint fs = glshader(GL_FRAGMENT_SHADER, fsrc);
  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char err[512];
    glGetProgramInfoLog(program, 512, NULL, err);
    fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n%s\n", err);
  }
  glDeleteShader(vs);
  glDeleteShader(fs);
  free(vsrc);
  free(fsrc);
  return program;
}
