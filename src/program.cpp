#include "program.h"
#include "shader.h"

Program::Program()
: _dirty(false), _gl_program(0)
{

}

Program::~Program()
{
    GL_CHECK(glDeleteProgram(_gl_program));
}

bool Program::add_shader(Shader *s)
{
    _dirty = true;
    _shaders.push_back(ref_ptr<Shader>(s));
    GL_CHECK(glAttachShader(_gl_program, s->_gl_shader));
    return true;
}

const Program::AttribLocationList & Program::get_active_attribs() const
{
    return _attrib_locations;
}

const Program::UniformLocationList & Program::get_active_uniforms() const
{
    return _uniform_locations;
}

bool Program::apply()
{
    if (_dirty)
    {
        GL_CHECK(glLinkProgram(_gl_program)); 

        GLint active_uniforms = 0;
        GL_CHECK(glGetProgramiv(_gl_program, GL_ACTIVE_UNIFORMS, &active_uniforms));
        GLint active_uniform_max_length = 0;
        GL_CHECK(glGetProgramiv(_gl_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_length));
        GLchar *name = new GLchar[active_uniform_max_length];
        for (GLint index = 0; index < active_uniforms; ++index)
        {
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = GL_NONE;
            GL_CHECK(glGetActiveUniform(_gl_program, index,
                active_uniform_max_length, &length, &size, &type, name));

            GLint location = GL_CHECK(glGetUniformLocation(_gl_program, name));
            _uniform_locations[name] = location;
        }
        delete [] name;
        
        GLint active_attribs = 0;
        GL_CHECK(glGetProgramiv(_gl_program, GL_ACTIVE_ATTRIBUTES, &active_attribs));
        GLint active_attrib_max_length = 0;
        GL_CHECK(glGetProgramiv(_gl_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &active_attrib_max_length));
        name = new GLchar[active_attrib_max_length];
        for (GLint index = 0; index < active_attribs; ++index)
        {
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = GL_NONE;
            GL_CHECK(glGetActiveAttrib(_gl_program, index,
                active_attrib_max_length, &length, &size, &type, name));

            GLint location = GL_CHECK(glGetAttribLocation(_gl_program, name));
            _attrib_locations[name] = location;
        }
        delete [] name;
    }

    GL_CHECK(glUseProgram(_gl_program));
}
