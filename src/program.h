#ifndef _INCLUDE_PROGRAM_
#define _INCLUDE_PROGRAM_

#include "GLES2/gl2.h"
#include "common.h"
#include "referenced.h"
#include "ref_ptr.h"

#include <vector>
#include <map>

class Shader;
class Program : public Referenced
{
public:
    typedef std::vector<ref_ptr<Shader> > ShaderList;
    typedef std::map<std::string, GLuint> AttribLocationList;
    typedef std::map<std::string, GLuint> UniformLocationList;

    Program();
    virtual ~Program();

    bool add_shader(Shader *s);
    bool apply();

    const AttribLocationList & get_active_attribs() const;
    const UniformLocationList & get_active_uniforms() const;

private:
    ShaderList _shaders;
    AttribLocationList _attrib_locations;
    UniformLocationList _uniform_locations;

    bool _dirty;
    GLuint _gl_program;
};

#endif // _INCLUDE_PROGRAM_
