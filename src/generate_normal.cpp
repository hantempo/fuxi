#include "objLoader/objLoader.h"
#include "math3d.h"

#include <fstream>

struct NormalEntry
{
    Vector3 sum;
    int count;
};

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        printf("Usage : %s src_obj dest_obj\n", argv[0]);
        exit(1);
    }

    const char * src_obj_filepath = argv[1];
    const char * dest_obj_filepath = argv[2];

    objLoader *objData = new objLoader;
    int loaded = objData->load(src_obj_filepath);
    FUXI_DEBUG_ASSERT(loaded, "Failed to load obj file");

    const int vertexCount = objData->vertexCount;
    const int faceCount = objData->faceCount;
    NormalEntry * entries = new NormalEntry[vertexCount];

    obj_vector **vertexList = objData->vertexList;
    for (int i = 0; i < faceCount; ++i)
    {
        const obj_face *face = objData->faceList[i];
        FUXI_DEBUG_ASSERT(face->vertex_count == 3, "Only accept triangles");

        const int i0 = face->vertex_index[0];
        const int i1 = face->vertex_index[1];
        const int i2 = face->vertex_index[2];

        const Vector3 normal = FaceNormal(
            Vector3(vertexList[i0]->e),
            Vector3(vertexList[i1]->e),
            Vector3(vertexList[i2]->e));

        entries[i0].sum += normal;
        entries[i0].count += 1;
        entries[i1].sum += normal;
        entries[i1].count += 1;
        entries[i2].sum += normal;
        entries[i2].count += 1;
    }

    for (int i = 0; i < vertexCount; ++i)
    {
        if (entries[i].count)
        {
            entries[i].sum /= entries[i].count;
            entries[i].count = 1;
        }
    }

    std::ofstream of(dest_obj_filepath);
    of << "# FUXI OBJ" << std::endl;
    for (int i = 0; i < vertexCount; ++i)
    {
        const obj_vector *v = vertexList[i];
        of << "v " << v->e[0] << " " << v->e[1]
           << " " << v->e[2] << std::endl;
    }
    for (int i = 0; i < vertexCount; ++i)
    {
        const Vector3 &n = entries[i].sum;
        of << "vn " << n.x << " " << n.y
           << " " << n.z << std::endl;
    }
    for (int i = 0; i < faceCount; ++i)
    {
        const obj_face *face = objData->faceList[i];
        const int i0 = face->vertex_index[0] + 1;
        const int i1 = face->vertex_index[1] + 1;
        const int i2 = face->vertex_index[2] + 1;
        of << "f " << i0 << "//" << i0
           << " "  << i1 << "//" << i1
           << " "  << i2 << "//" << i2 << std::endl;
    }
}
