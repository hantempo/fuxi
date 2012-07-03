#include "common.h"
#include "math3d.h"
#include "image.h"

#include "context.h"
#include "shader.h"
#include "geometry.h"

#ifndef GL_STENCIL_INDEX
#define GL_STENCIL_INDEX            0x1901
#endif

int main(int argc, char **argv) {

    const unsigned int WIDTH  = 1280;
    const unsigned int HEIGHT = 720;
    Context context(WIDTH, HEIGHT);

    float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.
    
    unsigned char *myPixels = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    unsigned char *myPixels2 = (unsigned char*)calloc(1, 128*128*4); // Holds texture data.
    
    const char* data_path_ptr = getenv("FUXI_DATA_PATH");
    FUXI_DEBUG_ASSERT_POINTER(data_path_ptr);
    const std::string data_path(data_path_ptr);

    GLuint fragShader, vertShader;
    process_shader(&vertShader, (data_path + "/shader/shader.vert").c_str(), GL_VERTEX_SHADER);
    process_shader(&fragShader, (data_path + "/shader/shader.frag").c_str(), GL_FRAGMENT_SHADER);

    GLuint program = GL_CHECK(glCreateProgram());
    GL_CHECK(glAttachShader(program, vertShader));
    GL_CHECK(glAttachShader(program, fragShader));
    GL_CHECK(glLinkProgram(program));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    GLint locPosition = GL_CHECK(glGetAttribLocation(program, "in_position"));
    GLint locNormal = GL_CHECK(glGetAttribLocation(program, "in_normal"));

    /* Get uniform locations */
    GLint locMVP = GL_CHECK(glGetUniformLocation(program, "mvp"));
    GLint locMV = GL_CHECK(glGetUniformLocation(program, "mv"));
    GLint locInvModel = GL_CHECK(glGetUniformLocation(program, "inv_model"));
    GLint locLightPos = GL_CHECK(glGetUniformLocation(program, "light_pos"));
    Vector3 light_pos(0.f, 0.f, 5.f);
    GL_CHECK(glUseProgram(program));
    
    GL_CHECK(glUniform3fv(locLightPos, 1, light_pos));

    // Load the model from obj file
    const char * obj_filename = argv[1];
    const std::string obj_filepath = data_path + "/model/" + obj_filename;
    const Geometry geometry(obj_filepath.c_str());
    FUXI_DEBUG_ASSERT(geometry.triangle_count(), "No faces in model.");

    geometry.enable_position_attribute(locPosition);
    geometry.enable_normal_attribute(locNormal);

    const Vector4 clear_color = Color::Black;
    GL_CHECK(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_STENCIL_TEST));
    GL_CHECK(glStencilFunc(GL_ALWAYS, 0, 0));
    GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR));
    
    /* Enter event loop */
    int iXangle = 0, iYangle = 0;
    int count = 0;
    float overdraw_ratio_sum = 0;
    while (count < 180)
    {
        const Matrix4x4 scale = Matrix4x4::Scale(Vector3(1, 1, 1));
        const Matrix4x4 rotateX = Matrix4x4::Rotate(Vector3(1, 0, 0), iXangle);
        const Matrix4x4 rotateY = Matrix4x4::Rotate(Vector3(0, 1, 0), iYangle);
        const Matrix4x4 translate = Matrix4x4::Translate(Vector3(0, -5, -10));
        const Matrix4x4 pers = Matrix4x4::Perspective(60.0f, (float)WIDTH/HEIGHT, 0.01, 100.0);
        const Matrix4x4 mv = scale * rotateX * rotateY * translate;
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
        glViewport(0, 0, WIDTH, HEIGHT);
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
         
        geometry.draw();

        // Get overdraw info
        Image stencil_framebuffer(WIDTH, HEIGHT, 1);
        GL_CHECK(glReadPixels(0, 0, WIDTH, HEIGHT, GL_STENCIL_INDEX,
            GL_UNSIGNED_BYTE, stencil_framebuffer.pixels));
        const float overdraw_ratio = stencil_framebuffer.overdraw_ratio();
        overdraw_ratio_sum += overdraw_ratio;
        char message[512];
        printf("Frame %d : Overdraw Ratio %f\n", count, overdraw_ratio);

        if (!context.swap_buffers()) 
        {
            printf("Failed to swap buffers.\n");
        }

        count++;
        usleep(20000);
    }
    
    printf("Total Frame Count : %d\n", count);
    printf("Average Overdraw Ratio : %f\n", overdraw_ratio_sum / count);

    GL_CHECK(glDeleteShader(vertShader));
    GL_CHECK(glDeleteShader(fragShader));
    GL_CHECK(glDeleteProgram(program));
    
    return 0;
}
