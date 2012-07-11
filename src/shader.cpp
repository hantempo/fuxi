#include "shader.h"
#include "common.h"

#include <fstream>
#include <streambuf>

Shader::Shader(Shader::Type type)
: _type(type), _gl_shader(0)
{
}

Shader::~Shader()
{
    GL_CHECK(glDeleteShader(_gl_shader));
}

bool Shader::load_source_from_file(const std::string &filename)
{
    std::ifstream ifs(filename.c_str());
    _source = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    _gl_shader = GL_CHECK(glCreateShader(_type));
    const char *source_array[1] = {_source.c_str()};
    GL_CHECK(glShaderSource(_gl_shader, 1, source_array, NULL));

	GL_CHECK(glCompileShader(_gl_shader));
    GLint status = GL_TRUE;
	GL_CHECK(glGetShaderiv(_gl_shader, GL_COMPILE_STATUS, &status));

	// Dump debug info (source and log) if compilation failed.
	if(status != GL_TRUE) {
		printf("Debug source START:\n%s\nDebug source END\n\n", _source.c_str());

        GLint length = 0;
		GL_CHECK(glGetShaderiv(_gl_shader, GL_INFO_LOG_LENGTH, &length));
		char *error_log = new char[length];
		GL_CHECK(glGetShaderInfoLog(_gl_shader, length, NULL, error_log));
		printf("Log START:\n%s\nLog END\n\n", error_log);
		delete [] error_log;
	}
}

Shader * Shader::ReadShaderFile(Shader::Type type, const std::string &filename)
{
    Shader * s = new Shader(type);
    if (s)
        s->load_source_from_file(filename);
    return s;
}

