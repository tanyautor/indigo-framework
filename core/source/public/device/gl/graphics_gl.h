#pragma once

// device specific, so keep them out of precomp.h
// should only be included in source files in the device folder

#include "imgui-master/backends/imgui_impl_glfw.h"
#include "imgui-master/backends/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "uniforms_gl.h"

// from learnopengl.com
GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 


void label_gl(GLenum type, GLuint name, const std::string& label);

// file parsing, need to abstract later
std::string GetBaseDir(const std::string& filepath);
bool FileExists(const std::string& abs_filename);
void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]);
bool LoadObjAndConvert(float bmin[3], float bmax[3],
    std::vector<Mesh>* meshes,
    std::vector<tinyobj::material_t>& materials,
    std::map<std::string, GLuint>& textures,
    const char* filename);

// from learnopengl.com (16.03.2026)
static void draw_quad(uint32 use_texture = 0)
{
    static Shader screen_pass{ "shared/shaders/screen_pass.vert" , "shared/shaders/screen_pass.frag" };
    static float quadVertices[] = { 
        // positions // texCoords
        -1.f,  -1.f,  0.0f, 0.0f,
         1.f,  -1.f,  1.0f, 0.0f,
        -1.f,   1.f,  0.0f, 1.0f,

        -1.f,   1.f,  0.0f, 1.0f,
         1.f,  -1.f,  1.0f, 0.0f,
         1.f,   1.f,  1.0f, 1.0f,
    };

    static unsigned int quadVAO{ 0 }, quadVBO{ 0 };
    if (!quadVAO)
    {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    screen_pass.use_shader();
    //glUniform1i(glGetUniformLocation(screen_pass.id(), "screenTexture"), 0);

    glBindVertexArray(quadVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glCheckError();
}