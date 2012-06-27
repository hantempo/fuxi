#include "geometry.h"
#include "objLoader/objLoader.h"

Geometry::Geometry(const char *obj_filepath)
: tri_count(0),
  position_vbo(0), normal_vbo(0),
  tex_coord_vbo(0), position_index_vbo(0),
  normal_index_vbo(0), tex_coord_index_vbo(0)
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

    // Load vertex attributes into VBO
    FUXI_DEBUG_ASSERT(objData->faceCount, "No face in the geometry");
    FUXI_DEBUG_ASSERT(objData->vertexCount, "No vertex position data.");
    const int position_float_num = objData->vertexCount * 3;
    float *position = new float[position_float_num];
    for (int i = 0; i < objData->vertexCount; ++i)
    {
        const obj_vector *pos = objData->vertexList[i];
        memcpy(position + i * 3, pos->e, sizeof(float) * 3);
    }
    GL_CHECK(glGenBuffers(1, &position_vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, position_vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * position_float_num, position, GL_STATIC_DRAW));
    delete [] position;

    if (objData->normalCount)
    {
        const int normal_float_num = objData->normalCount * 3;
        float *normal = new float[normal_float_num];
        for (int i = 0; i < objData->normalCount; ++i)
        {
            const obj_vector *n = objData->normalList[i];
            memcpy(normal + i * 3, n->e, sizeof(float) * 3);
        }
        GL_CHECK(glGenBuffers(1, &normal_vbo));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, normal_vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normal_float_num, normal, GL_STATIC_DRAW));
        delete [] normal;
    }
    
    if (objData->textureCount)
    {
        const int texture_float_num = objData->textureCount * 2;
        float *texture = new float[texture_float_num];
        for (int i = 0; i < objData->textureCount; ++i)
        {
            const obj_vector *t = objData->textureList[i];
            memcpy(texture + i * 2, t->e, sizeof(float) * 2);
        }
        GL_CHECK(glGenBuffers(1, &tex_coord_vbo));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, tex_coord_vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texture_float_num, texture, GL_STATIC_DRAW));
        delete [] texture;
    }

    unsigned short *pos_index = new unsigned short[3 * objData->faceCount];
    for (int i = 0; i < objData->faceCount; ++i)
    {
        obj_face *f = objData->faceList[i];
        FUXI_DEBUG_ASSERT(f->vertex_count == 3, "Only accept triangle face");
        pos_index[i * 3 + 0] = static_cast<unsigned short>(f->vertex_index[0]);
        pos_index[i * 3 + 1] = static_cast<unsigned short>(f->vertex_index[1]);
        pos_index[i * 3 + 2] = static_cast<unsigned short>(f->vertex_index[2]);
    }
    GL_CHECK(glGenBuffers(1, &position_index_vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, position_index_vbo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * objData->faceCount, pos_index, GL_STATIC_DRAW));
    delete [] pos_index;

    tri_count = objData->faceCount;
}

Geometry::~Geometry()
{
    GL_CHECK(glDeleteBuffers(1, &position_vbo));
    GL_CHECK(glDeleteBuffers(1, &normal_vbo));
    GL_CHECK(glDeleteBuffers(1, &tex_coord_vbo));
    GL_CHECK(glDeleteBuffers(1, &position_index_vbo));
    GL_CHECK(glDeleteBuffers(1, &normal_index_vbo));
    GL_CHECK(glDeleteBuffers(1, &tex_coord_index_vbo));
}

unsigned int Geometry::triangle_count() const
{
    return tri_count;
}

void Geometry::enable_position_attribute(GLuint index) const
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, position_vbo));
    GL_CHECK(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0));
}

void Geometry::draw() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, position_index_vbo));
    GL_CHECK(glDrawElements(GL_TRIANGLES, tri_count * 3, GL_UNSIGNED_SHORT, 0));
}
