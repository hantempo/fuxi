#include "common.h"
#include "math3d.h"
#include "image.h"
#include "cache.h"

#include "context.h"
#include "shader.h"
#include "program.h"
#include "geometry.h"
#include "ref_ptr.h"

#ifndef GL_STENCIL_INDEX
#define GL_STENCIL_INDEX            0x1901
#endif

const UInt8 POST_TRANSFORM_CACHE_SIZE = 32;
const UInt32 INITIAL_WIDTH = 10;
const UInt32 INITIAL_HEIGHT = 10;

int main(int argc, char **argv)
{
    Context context(INITIAL_WIDTH, INITIAL_HEIGHT);

    ///////////////////////////////////////////////////////////////////////////
    // Load shaders
    ///////////////////////////////////////////////////////////////////////////
    const char* data_path_ptr = getenv("FUXI_DATA_PATH");
    FUXI_DEBUG_ASSERT_POINTER(data_path_ptr);
    const std::string data_path(data_path_ptr);

    ref_ptr<Program> p = new Program;
    p->add_shader(Shader::ReadShaderFile(Shader::VERTEX, data_path + "/shader/shader.vert"));
    p->add_shader(Shader::ReadShaderFile(Shader::FRAGMENT, data_path + "/shader/shader.frag"));
    p->apply();
    Program::UniformLocationList &uniform_locations = const_cast<Program::UniformLocationList &>(p->get_active_uniforms());
    Program::AttribLocationList &attrib_locations = const_cast<Program::AttribLocationList &>(p->get_active_attribs());
    
    const GLint locPosition = attrib_locations["in_position"];
    const GLint locNormal = attrib_locations["in_normal"];
    const GLint locMVP = uniform_locations["mvp"];
    const GLint locMV = uniform_locations["mv"];
    const GLint locInvModel = uniform_locations["inv_model"];
    const GLint locLightPos = uniform_locations["light_pos"];

    Vector3 light_pos(0.f, 0.f, 5.f);
    GL_CHECK(glUniform3fv(locLightPos, 1, light_pos));

    ///////////////////////////////////////////////////////////////////////////
    // Load model
    ///////////////////////////////////////////////////////////////////////////
    const char * obj_filename = argv[1];
    ref_ptr<Geometry> geometry = new Geometry(obj_filename);
    const vertex_index_type vertex_count = geometry->vertex_count();
    const face_index_type tri_count = geometry->triangle_count();
    FUXI_DEBUG_ASSERT(tri_count, "No faces in model.");
    
    // check the ACMR and ATVR before optimization
    {
        FIFOCache<vertex_index_type> cache(POST_TRANSFORM_CACHE_SIZE);
        const vertex_index_type *v = geometry->index_list();
        cache.load(v, v + tri_count * 3);
        const UInt32 miss_count = cache.get_miss_count();
        const UInt32 load_count = cache.get_load_count();

        printf("\n");
        printf("Before optimization:\n");
        printf("Miss Count : %d \nLoad Count : %d\n", miss_count, load_count);
        printf("ACMR : %f\n", (Float32)miss_count / tri_count);
        printf("ATVR : %f\n", (Float32)miss_count / vertex_count);
    }

    //Tipsify(geometry->index_list(), geometry->vertex_count(),
        //geometry->triangle_count(), POST_TRANSFORM_CACHE_SIZE);

    {
        FIFOCache<vertex_index_type> cache(POST_TRANSFORM_CACHE_SIZE);
        const vertex_index_type *v = geometry->index_list();
        cache.load(v, v + tri_count * 3);
        const UInt32 miss_count = cache.get_miss_count();
        const UInt32 load_count = cache.get_load_count();

        printf("\n");
        printf("After optimization:\n");
        printf("Miss Count : %d \nLoad Count : %d\n", miss_count, load_count);
        printf("ACMR : %f\n", (Float32)miss_count / tri_count);
        printf("ATVR : %f\n", (Float32)miss_count / vertex_count);
    }

    geometry->enable_position_attribute(locPosition);
    geometry->enable_normal_attribute(locNormal);

    const Vector4 clear_color = Color::Black;
    GL_CHECK(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_STENCIL_TEST));
    GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0));
    GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));

    ///////////////////////////////////////////////////////////////////////////
    // Set viewport transformation according to model bounding volumn
    ///////////////////////////////////////////////////////////////////////////
    const AABB &bb = geometry->get_bounding_box();
    const Vector3 &center = bb.center();
    const Vector3 &dim = bb.dimension();
    const Vector3 &eye = center + Vector3(0.f, 0.f, dim.y * 0.7f);
    const UInt32 height = 600;
    const UInt32 width = static_cast<UInt32>(height * (dim.x / dim.y));
    context.resize(width, height);
    
    /* Enter event loop */
    int iXangle = 0, iYangle = 0;
    int count = 0;
    float overdraw_ratio_sum = 0;
    Image stencil_framebuffer(width, height, 1);
    while (count < 180)
    {
        const Matrix4x4 rotateX = Matrix4x4::Rotate(Vector3(1, 0, 0), iXangle);
        const Matrix4x4 rotateY = Matrix4x4::Rotate(Vector3(0, 1, 0), iYangle);
        const Matrix4x4 lookat = Matrix4x4::LookAt(eye, center, Vector3(0, 1, 0));
        const Matrix4x4 pers = Matrix4x4::Perspective(60.0f, (float)width/height, 0.01, 100.0);
        const Matrix4x4 mv = rotateX * rotateY * lookat;
        const Matrix4x4 inv_model = Matrix4x4::Transpose(Matrix4x4::Invert4x3(mv));
        const Matrix4x4 mvp = mv * pers;
        GL_CHECK(glUniformMatrix4fv(locMVP, 1, GL_FALSE, mvp));
        GL_CHECK(glUniformMatrix4fv(locMV, 1, GL_FALSE, mv));
        GL_CHECK(glUniformMatrix4fv(locInvModel, 1, GL_FALSE, inv_model));

        if (count < 180)
        {
            iYangle += 2;
            if(iYangle >= 360) iYangle -= 360;
            if(iYangle < 0) iYangle += 360;
        }
        //else
        //{
            //iXangle += 2;
            //if(iXangle >= 360) iXangle -= 360;
            //if(iXangle < 0) iXangle += 360;
        //}

        ///////////////////////////////////////////////////////////////////
        glViewport(0, 0, width, height);
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
         
        geometry->draw();

        // Get overdraw info
        GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        GL_CHECK(glReadPixels(0, 0, width, height, GL_STENCIL_INDEX,
                    GL_UNSIGNED_BYTE, stencil_framebuffer.pixels));
        //const float overdraw_ratio = stencil_framebuffer.overdraw_ratio();
        //overdraw_ratio_sum += overdraw_ratio;
        //char message[512];
        //printf("Frame %d : Overdraw Ratio %f\n", count, overdraw_ratio);

        if (!context.swap_buffers()) 
        {
            printf("Failed to swap buffers.\n");
        }

        count++;
        usleep(20000);
    }
    
    //printf("Total Frame Count : %d\n", count);
    //printf("Average Overdraw Ratio : %f\n", overdraw_ratio_sum / count);

    return 0;
}
