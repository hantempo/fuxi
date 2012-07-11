#ifndef _INCLUDE_GEOMETRY_
#define _INCLUDE_GEOMETRY_

#include "common.h"
#include "math3d.h"
#include "referenced.h"
#include <GLES2/gl2.h>
#include <EGL/egl.h>

typedef UInt32 face_index_type;
typedef UInt16 vertex_index_type;
typedef Float32 attribute_type;

// Only accept triangle meshes
class Geometry : public Referenced
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
    
    void generate_normal();

    void draw() const;

    const AABB & get_bounding_box() const { return bounding_box; }

    // According to "Fast Triangle Reordering for Vertex Locality and Reduced Overdraw"
    void reorder_triangles(UInt8 cache_size);

private:
    void generate_normals();
    void build_adjacent_relationship();

    UInt8 adjacent_triangle_count(vertex_index_type vi) const;
    face_index_type adjacent_triangle(vertex_index_type vi, UInt8 fi) const;

    face_index_type tri_count;
    vertex_index_type v_count;

    Vector3 *positions, *normals;
    vertex_index_type *indices;

    GLuint attributes_vbo;
    GLuint indices_vbo;
    UInt8 position_channels;
    UInt8 normal_channels;
    UInt8 tex_coord_channels;

    // The count of adjacent triangles for each vertex
    UInt8 *adj_tri_count;
    // The triangle-list offset map for each vertex
    face_index_type *tri_offset;
    face_index_type *tri_list;

    AABB bounding_box;
};

void Tipsify(vertex_index_type * indices,
    vertex_index_type vertex_count,
    face_index_type face_count,
    UInt8 cache_size);

#endif // _INCLUDE_GEOMETRY_
