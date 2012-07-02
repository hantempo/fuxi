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

    // According to "Fast Triangle Reordering for Vertex Locality and Reduced Overdraw"
    void reorder_triangles();

private:
    unsigned int tri_count;

    float * attributes;
    unsigned short * indices;

    GLuint attributes_vbo;
    GLuint indices_vbo;
    unsigned int position_channels;
    unsigned int normal_channels;
    unsigned int tex_coord_channels;

};

#endif // _INCLUDE_GEOMETRY_
