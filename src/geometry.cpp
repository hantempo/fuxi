#include "geometry.h"
#include "objLoader/objLoader.h"

#include <stack>
#include <vector>
#include <set>
#include <limits>
#include <iostream>
#include <algorithm>

template <typename Iter>
void print_range(Iter begin, Iter end)
{
    for (Iter iter = begin; iter != end; ++iter)
    {
        std::cout << (int)(*iter) << std::endl;
    }
}

Geometry::Geometry(const char *obj_filepath)
: tri_count(0), v_count(0),
  attributes(NULL), indices(NULL),
  attributes_vbo(0), indices_vbo(0),
  position_channels(3), normal_channels(3),
  tex_coord_channels(0)
{
    objLoader *objData = new objLoader();
    int loaded = objData->load(obj_filepath);
    FUXI_DEBUG_ASSERT(loaded, "Failed to load obj file");

#ifndef NDEBUG
    printf("Loaded OBJ file: %s\n", obj_filepath);
    printf("Number of vertices: %i\n", objData->vertexCount);
    printf("Number of vertex normals: %i\n", objData->normalCount);
    printf("Number of texture coordinates: %i\n", objData->textureCount);
    printf("Number of faces: %i\n", objData->faceCount);
#endif
    
    FUXI_DEBUG_ASSERT(objData->vertexCount < std::numeric_limits<UInt16>::max(),
        "Vertex index can't be expressed with unsigned short");

    // Load vertex attributes into VBO
    tri_count = objData->faceCount;
    v_count = objData->vertexCount;
    const UInt8 totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const UInt8 vertexStride = totalChannels * sizeof(Float32);

    FUXI_DEBUG_ASSERT(tri_count, "No face in the geometry");
    FUXI_DEBUG_ASSERT(v_count, "No vertex position data.");
    FUXI_DEBUG_ASSERT(objData->normalCount, "No normal data");
    FUXI_DEBUG_ASSERT(objData->normalCount == v_count, "It's not normal per vertex");

    attributes = new attribute_type[v_count * totalChannels];
    attribute_type *patrol = attributes;
    for (vertex_index_type i = 0; i < v_count; ++i)
    {
        const obj_vector *pos = objData->vertexList[i];
        *(patrol++) = (pos->e)[0];
        *(patrol++) = (pos->e)[1];
        *(patrol++) = (pos->e)[2];

        const obj_vector *n = objData->normalList[i];
        *(patrol++) = (n->e)[0];
        *(patrol++) = (n->e)[1];
        *(patrol++) = (n->e)[2];
    }
    GL_CHECK(glGenBuffers(1, &attributes_vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attributes_vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexStride * v_count,
        attributes, GL_STATIC_DRAW));

    indices = new vertex_index_type[3 * tri_count];
    for (face_index_type i = 0; i < tri_count; ++i)
    {
        const obj_face *f = objData->faceList[i];
        FUXI_DEBUG_ASSERT(f->vertex_count == 3, "Only accept triangle face");
        indices[i * 3 + 0] = static_cast<vertex_index_type>(f->vertex_index[0]);
        indices[i * 3 + 1] = static_cast<vertex_index_type>(f->vertex_index[1]);
        indices[i * 3 + 2] = static_cast<vertex_index_type>(f->vertex_index[2]);
    }
    GL_CHECK(glGenBuffers(1, &indices_vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(UInt16) * 3 * tri_count, indices, GL_STATIC_DRAW));
}

Geometry::~Geometry()
{
    delete [] attributes;
    delete [] indices;

    GL_CHECK(glDeleteBuffers(1, &attributes_vbo));
    GL_CHECK(glDeleteBuffers(1, &indices_vbo));
}

void Geometry::enable_position_attribute(GLuint index) const
{
    const UInt8 totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const UInt8 vertexStride = totalChannels * sizeof(Float32);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attributes_vbo));
    GL_CHECK(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, vertexStride, 0));
    GL_CHECK(glEnableVertexAttribArray(index));
}

void Geometry::enable_normal_attribute(GLuint index) const
{
    const UInt8 totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const UInt8 vertexStride = totalChannels * sizeof(Float32);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attributes_vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo));
    GL_CHECK(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE,
        vertexStride,
        (GLfloat *)(NULL) + position_channels));
    GL_CHECK(glEnableVertexAttribArray(index));
}

void Geometry::enable_tex_coord_attribute(GLuint index) const
{
    FUXI_DEBUG_ASSERT(0, "Not implemented");
}

void Geometry::draw() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo));
    GL_CHECK(glDrawElements(GL_TRIANGLES, tri_count * 3, GL_UNSIGNED_SHORT, 0));
}

vertex_index_type get_next_vertex(const vertex_index_type *indices,
        vertex_index_type vertex_count,
        vertex_index_type &cursor, const UInt8 cache_size,
        const std::set<vertex_index_type> &candidates,
        const UInt32 *cache_time, const UInt32 time,
        const std::vector<UInt8> &adj_count,
        std::stack<vertex_index_type> &visited_stack)
{
    SInt16 next = std::numeric_limits<vertex_index_type>::max();
    SInt32 priority = -1;
    SInt32 max_priority = -1;
    std::set<vertex_index_type>::const_iterator citer = candidates.begin();
    for (; citer != candidates.end(); ++citer)
    {
        const vertex_index_type v = *citer;
        if (adj_count[v] > 0)
        {
            priority = 0;
            if (time - cache_time[v] + 2 * adj_count[v] <= cache_size)
                priority = time - cache_time[v];
            if (priority > max_priority)
            {
                max_priority = priority;
                next = v;
            }
        }
    }

    // Pop from the visited stack
    if (next == std::numeric_limits<vertex_index_type>::max())
    {
        while (visited_stack.size())
        {
            const vertex_index_type v = visited_stack.top();
            visited_stack.pop();
            if (adj_count[v])
            {
                next = v;
                break;
            }
        }
    }
    
    // Cursor sweep the vertex list
    if (next == std::numeric_limits<vertex_index_type>::max())
    {
        while (cursor < vertex_count)
        {
            if (adj_count[cursor] > 0)
                break;
            ++cursor;
        }
    }

    return next;
}

void Tipsify(vertex_index_type * indices,
    vertex_index_type vertex_count,
    face_index_type face_count,
    UInt8 cache_size)
{
    // Build up vertex-triangle connectivity
    std::vector<UInt8> adj_count(vertex_count, 0);
    face_index_type *tri_offset = new face_index_type[vertex_count];
    face_index_type *tri_list = new face_index_type[face_count * 3];

    // Count the triangle numbers for each vertex
    for (face_index_type i = 0; i < face_count * 3; ++i)
    {
        ++adj_count[indices[i]];
    }
    // Calculate the offset of triangle list for each vertex
    for (vertex_index_type i = 0; i < vertex_count; ++i)
    {
        if (i == 0)
            tri_offset[i] = 0;
        else
            tri_offset[i] = tri_offset[i - 1] + adj_count[i - 1];
    }
    // Build up triangle list
    adj_count.assign(vertex_count, 0);
    for (face_index_type i = 0; i < face_count * 3; ++i)
    {
        const face_index_type offset = tri_offset[indices[i]] + adj_count[indices[i]]++;
        tri_list[offset] = i / 3;
    }

    // Per-vertex caching time stamps
    UInt32 *cache_time = new UInt32[vertex_count];
    memset(cache_time, 0, sizeof(UInt32) * vertex_count);
    // Dead-end vertex stack
    std::stack<vertex_index_type> visited_stack;
    // Per-triangle emitted flag
    UInt8 *tri_emitted = new UInt8[face_count];
    memset(tri_emitted, 0, sizeof(UInt8) * face_count);
    // New index buffer
    std::vector<vertex_index_type> new_indices;
    new_indices.reserve(face_count * 3);

    vertex_index_type vertex = 0;
    UInt32 time = cache_size + 1;
    vertex_index_type cursor = 1;

    while (vertex >= 0)
    {
        std::set<vertex_index_type> candidates;
        const face_index_type offset = tri_offset[vertex];
        face_index_type next_offset = 0;
        if (vertex + 1 == vertex_count)
            next_offset = face_count * 3;
        else
            next_offset = tri_offset[vertex + 1];
        for (face_index_type i = offset; i < next_offset; ++i)
        {
            const face_index_type f = tri_list[i];
            if (!tri_emitted[f])
            {
                const vertex_index_type *v_begin = indices + f * 3;
                const vertex_index_type *v_end = v_begin + 3;
                for (const vertex_index_type *p = v_begin; p != v_end; ++p)
                {
                    const vertex_index_type v = *p;
                    new_indices.push_back(v);
                    visited_stack.push(v);
                    candidates.insert(v);
                    --adj_count[v];
                    if ((time - cache_time[v]) > cache_size)
                    {
                        // if out of cache, this vertex will be
                        // processed and loaded again
                        cache_time[v] = time;
                        ++time;
                    }
                }
                tri_emitted[f] = 1;
            }
        }
        vertex = get_next_vertex(indices, vertex_count, cursor, cache_size,
                candidates, cache_time, time, adj_count, visited_stack);

        if (vertex == std::numeric_limits<vertex_index_type>::max())
            break;
    }

    std::copy(new_indices.begin(), new_indices.end(), indices);

    delete [] cache_time;
    delete [] tri_emitted;

    delete [] tri_offset;
    delete [] tri_list;
}
