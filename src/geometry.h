#ifndef _INCLUDE_GEOMETRY_
#define _INCLUDE_GEOMETRY_

#include "common.h"
#include <GLES2/gl2.h>
#include <EGL/egl.h>

typedef UInt32 face_index_type;
typedef UInt16 vertex_index_type;
typedef Float32 attribute_type;

// Only accept triangle meshes
class Geometry
{
public:
    Geometry(const char *obj_filepath);
    virtual ~Geometry();

    face_index_type triangle_count() const { return tri_count; }
    vertex_index_type vertex_count() const { return v_count; }
    vertex_index_type *index_list() const { return indices; }
    
    void enable_position_attribute(GLuint index) const;
    void enable_normal_attribute(GLuint index) const;
    void enable_tex_coord_attribute(GLuint index) const;

    void draw() const;

    // According to "Fast Triangle Reordering for Vertex Locality and Reduced Overdraw"
    void reorder_triangles(UInt8 cache_size);

private:
    face_index_type tri_count;
    vertex_index_type v_count;

    attribute_type * attributes;
    vertex_index_type * indices;

    GLuint attributes_vbo;
    GLuint indices_vbo;
    UInt8 position_channels;
    UInt8 normal_channels;
    UInt8 tex_coord_channels;
};

void Tipsify(vertex_index_type * indices,
    vertex_index_type vertex_count,
    face_index_type face_count,
    UInt8 cache_size);

#endif // _INCLUDE_GEOMETRY_
