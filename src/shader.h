#ifndef SHADER_H
#define SHADER_H

#include "GLES2/gl2.h"
#include "referenced.h"
#include "common.h"

class Shader : public Referenced
{
public:
    enum Type
    {
        VERTEX      = GL_VERTEX_SHADER,
        FRAGMENT    = GL_FRAGMENT_SHADER,
        UNDEFINED   = -1,
    };

    Shader(Type type = UNDEFINED);
    virtual ~Shader();

    bool load_source_from_file(const std::string &filename);

    static Shader * ReadShaderFile(Type type, const std::string &filename);

private:
    friend class Program;

    Type _type;
    std::string _filename;
    std::string _source;
    GLuint _gl_shader;
};

#endif

