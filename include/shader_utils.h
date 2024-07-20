#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GL/glew.h>

GLuint glprogram(const char* vpath, const char *fpath);

#endif
