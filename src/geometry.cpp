#include "geometry.h"
#include "objLoader/objLoader.h"

Geometry::Geometry(const char *obj_filepath)
: tri_count(0),
  attributes_vbo(0), index_vbo(0),
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

    // Load vertex attributes into VBO
    const int faceCount = objData->faceCount;
    const int vertexCount = objData->vertexCount;
    const int totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const int vertexStride = totalChannels * sizeof(float);

    FUXI_DEBUG_ASSERT(faceCount, "No face in the geometry");
    FUXI_DEBUG_ASSERT(vertexCount, "No vertex position data.");
    FUXI_DEBUG_ASSERT(objData->normalCount, "No normal data");
    FUXI_DEBUG_ASSERT(objData->normalCount == vertexCount, "It's not normal per vertex");

    float *attributes = new float[vertexCount * totalChannels];
    float *patrol = attributes;
    for (int i = 0; i < vertexCount; ++i)
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
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexStride * vertexCount,
        attributes, GL_STATIC_DRAW));
    delete [] attributes;

    unsigned short *index = new unsigned short[3 * faceCount];
    for (int i = 0; i < faceCount; ++i)
    {
        const obj_face *f = objData->faceList[i];
        FUXI_DEBUG_ASSERT(f->vertex_count == 3, "Only accept triangle face");
        index[i * 3 + 0] = static_cast<unsigned short>(f->vertex_index[0]);
        index[i * 3 + 1] = static_cast<unsigned short>(f->vertex_index[1]);
        index[i * 3 + 2] = static_cast<unsigned short>(f->vertex_index[2]);
    }
    GL_CHECK(glGenBuffers(1, &index_vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 3 * faceCount, index, GL_STATIC_DRAW));
    delete [] index;

    tri_count = objData->faceCount;
}

Geometry::~Geometry()
{
    GL_CHECK(glDeleteBuffers(1, &attributes_vbo));
    GL_CHECK(glDeleteBuffers(1, &index_vbo));
}

unsigned int Geometry::triangle_count() const
{
    return tri_count;
}

void Geometry::enable_position_attribute(GLuint index) const
{
    const int totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const int vertexStride = totalChannels * sizeof(float);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attributes_vbo));
    GL_CHECK(glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, vertexStride, 0));
    GL_CHECK(glEnableVertexAttribArray(index));
}

void Geometry::enable_normal_attribute(GLuint index) const
{
    const int totalChannels = (position_channels + normal_channels +
        tex_coord_channels);
    const int vertexStride = totalChannels * sizeof(float);

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attributes_vbo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo));
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
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo));
    GL_CHECK(glDrawElements(GL_TRIANGLES, tri_count * 3, GL_UNSIGNED_SHORT, 0));
}
