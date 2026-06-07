#include "precomp.h"

#ifdef BACKEND_OPENGL

bool ImGui_Impl_Init()
{
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(engine.get_window()->get_window());
    const bool opengl = ImGui_ImplOpenGL3_Init();
    const bool glfw = ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplGlfw_InstallCallbacks(window);
    return glfw && opengl;
}

void ImGui_Impl_Shutdown()
{
    ImGui_ImplGlfw_Shutdown();
}

void ImGui_Impl_NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGui_Impl_RenderDrawData(ImDrawData* _data)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui_ImplOpenGL3_RenderDrawData(_data);
}

#endif
