#include "precomp.h"

// private Impl
struct Renderer::Impl
{

};

// device specific definitions
Renderer::Renderer() :  Module()
{
    name = "Renderer";
    manual_interface = true;

    default_framebuffer = std::make_shared<Framebuffer>("Viewport");
    default_framebuffer->init_color_buffer(GL_TEXTURE_2D);
    default_framebuffer->init_depth_stencil(GL_RENDERBUFFER);

    // assign and check for completion of viewport
    engine.get_window()->viewports.push_back(default_framebuffer);
    assert(default_framebuffer->check_complete());
}

Renderer::~Renderer()
{
    for (size_t i = 0; i < subsystems.size(); i++)
    {
        delete subsystems[i];
    }

    bool empty = subsystems.empty();
}

void Renderer::interface_window()
{
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::Begin(get_title().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar);

    // resize viewport if needed
    float width = ImGui::GetWindowWidth();
    float height = ImGui::GetWindowHeight();
    auto window = engine.get_window();
    if ((uint32)width != default_framebuffer->width || (uint32)height != default_framebuffer->height)
    {
        window->resize_viewport((uint32)width, (uint32)height);
    }
    const auto lm = static_cast<uint64>(default_framebuffer->fbo);
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    ImGui::Image(lm, ImVec2(width, height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    auto btnColor = colors[ImGuiCol_Button];
    btnColor.w *= 0.4f;
    const float UIScale = 1.0f;  // Game.Device().GetMonitorUIScale();
    const auto s = ImGui::GetIO().FontGlobalScale * UIScale;
    const ImVec2 btnSize(24.0f * s, 24.0f * s);
    ImGui::SetCursorPos(ImVec2(6.0f, 6.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, btnColor);

    ImGui::PopStyleColor();

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}

std::shared_ptr<Framebuffer> Renderer::get_default_framebuffer()
{
    return default_framebuffer;
}

void Renderer::bind_default_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, default_framebuffer->fbo);
}

void Renderer::bind_backbuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::begin_frame()
{
    // Clear Backbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}
void Renderer::end_frame()
{
    // screen pass to back buffer
    bind_backbuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(default_framebuffer->color_buffer_access, default_framebuffer->color_buffer.value());
    draw_quad();

    glCheckError();
}