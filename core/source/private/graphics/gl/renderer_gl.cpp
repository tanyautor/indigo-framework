#include "precomp.h"

// private Impl
struct Renderer::Impl
{

};

// device specific definitions
Renderer::Renderer()
{
    default_framebuffer = new Framebuffer("Default Framebuffer");
    default_framebuffer->init_color_buffer(GL_TEXTURE_2D);
    default_framebuffer->init_depth_stencil(GL_RENDERBUFFER);
}

Renderer::~Renderer()
{
    for (size_t i = 0; i < subsystems.size(); i++)
    {
        delete subsystems[i];
    }

    bool empty = subsystems.empty();
}

Framebuffer* Renderer::get_default_framebuffer()
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