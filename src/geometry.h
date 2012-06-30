#ifndef _INCLUDE_GEOMETRY_
#define _INCLUDE_GEOMETRY_

#include "common.h"
#include <GLES2/gl2.h>
#include <EGL/egl.h>

// Only accept triangle meshes
class Geometry
{
public:
    Geometry(const char *obj_filepath);
    virtual ~Geometry();

    unsigned int triangle_count() const;
    
    void enable_position_attribute(GLuint index) const;
    void enable_normal_attribute(GLuint index) const;
    void enable_tex_coord_attribute(GLuint index) const;

    void draw() const;

private:
    unsigned int tri_count;

    GLuint position_vbo;
    GLuint normal_vbo;
    GLuint tex_coord_vbo;
    GLuint position_index_vbo;
    GLuint normal_index_vbo;
    GLuint tex_coord_index_vbo;
};

#endif // _INCLUDE_GEOMETRY_
