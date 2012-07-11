#ifndef SHADER_H
#define SHADER_H

#include "GLES2/gl2.h"

char* load_shader(const char *sFilename);
void process_shader(GLuint *pShader, const char *sFilename, GLint iShaderType);

#endif

