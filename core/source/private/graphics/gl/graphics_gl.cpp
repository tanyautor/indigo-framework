#include "precomp.h"

// callbacks
static void error_callback(int error, const char* description)
{
    log(Error, "glfw msg: %s", description);
}
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    // resized viewport currently used
    for (auto buffer : engine.get_window()->viewports)
    {
        buffer->resize_framebuffer(width, height);
    }

    if(height > 0)
        engine.get_active_camera()->projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, 0.1f, 100.f);
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        log(Error, "{} | {} ({})", error, file, line);
    }
    return errorCode;
}

void label_gl(GLenum type, GLuint name, const std::string& label)
{
    std::string typeString;
    switch (type)
    {
    case GL_BUFFER:
        typeString = "GL_BUFFER";
        break;
    case GL_SHADER:
        typeString = "GL_SHADER";
        break;
    case GL_PROGRAM:
        typeString = "GL_PROGRAM";
        break;
    case GL_VERTEX_ARRAY:
        typeString = "GL_VERTEX_ARRAY";
        break;
    case GL_QUERY:
        typeString = "GL_QUERY";
        break;
    case GL_PROGRAM_PIPELINE:
        typeString = "GL_PROGRAM_PIPELINE";
        break;
    case GL_TRANSFORM_FEEDBACK:
        typeString = "GL_TRANSFORM_FEEDBACK";
        break;
    case GL_SAMPLER:
        typeString = "GL_SAMPLER";
        break;
    case GL_TEXTURE:
        typeString = "GL_TEXTURE";
        break;
    case GL_RENDERBUFFER:
        typeString = "GL_RENDERBUFFER";
        break;
    case GL_FRAMEBUFFER:
        typeString = "GL_FRAMEBUFFER";
        break;
    default:
        typeString = "UNKNOWN";
        break;
    }

    const std::string temp = "[" + typeString + ":" + std::to_string(name) + "] " + label;
    glObjectLabel(type, name, static_cast<GLsizei>(temp.length()), temp.c_str());
}

Framebuffer::Framebuffer(const char* _rdg_label) : rdg_label{"BoringBuffer_>:("}
{
    glGenFramebuffers(1, &fbo);

    width = engine.get_window()->get_window_size().x;
    height = engine.get_window()->get_window_size().y;

    if(_rdg_label)
    {
        rdg_label = std::string(_rdg_label);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        label_gl(GL_FRAMEBUFFER, fbo, rdg_label.c_str());
        glCheckError();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
Framebuffer::~Framebuffer()
{
    if (color_buffer.has_value())
    {
        if(color_buffer_access == GL_TEXTURE_2D)    glDeleteTextures(1, &color_buffer.value());
        if(color_buffer_access == GL_RENDERBUFFER)  glDeleteRenderbuffers(1, &color_buffer.value());
    }

    if (depth_stencil.has_value()) 
    {
        if (depth_stencil_access == GL_TEXTURE_2D)    glDeleteTextures(1, &depth_stencil.value());
        if (depth_stencil_access == GL_RENDERBUFFER)  glDeleteRenderbuffers(1, &depth_stencil.value());
    }

    glDeleteFramebuffers(1, &fbo);
}
bool Framebuffer::init_color_buffer(uint32 _access)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // delete existing buffer
    if (color_buffer.has_value())
    {
        uint32 _buffer[1]{ color_buffer.value() };
        glDeleteTextures(1, _buffer);
        glCheckError();
    }

    color_buffer = 0;
    color_buffer_access = _access;

    // manage access in a... weird way
    if (_access == GL_TEXTURE_2D)
    {
        glGenTextures(1, &color_buffer.value());
        glBindTexture(GL_TEXTURE_2D, color_buffer.value());

        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA32F,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer.value(), 0);
        label_gl(GL_TEXTURE, color_buffer.value(), std::string(rdg_label + " ColorBuffer").c_str());
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if(_access == GL_RENDERBUFFER)
    {
        glGenRenderbuffers(1, &color_buffer.value());
        glBindRenderbuffer(GL_RENDERBUFFER, color_buffer.value());

        glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, color_buffer.value());

        label_gl(GL_RENDERBUFFER, color_buffer.value(), std::string(rdg_label + " ColorBuffer").c_str());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    glCheckError();

    return color_buffer.has_value();
}
bool Framebuffer::init_depth_stencil(uint32 _access)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // delete existing buffer
    if (depth_stencil.has_value())
    {
        uint32 _buffer[1]{ depth_stencil.value() };
        glDeleteTextures(1, _buffer);
        glCheckError();
    }

    depth_stencil = 0;
    depth_stencil_access = _access;

    if (_access == GL_TEXTURE_2D)
    {
        glGenTextures(1, &depth_stencil.value());
        glBindTexture(GL_TEXTURE_2D, depth_stencil.value());

        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA32F,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depth_stencil.value(), 0);
        label_gl(GL_TEXTURE, depth_stencil.value(), std::string(rdg_label + " Depth-Stencil").c_str());
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if (_access == GL_RENDERBUFFER)
    {
        glGenRenderbuffers(1, &depth_stencil.value());
        glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil.value());

        glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil.value());

        label_gl(GL_RENDERBUFFER, depth_stencil.value(), std::string(rdg_label + " Depth-Stencil").c_str());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glCheckError();

    return depth_stencil.has_value();
}
bool Framebuffer::check_complete()
{
    // check if framebuffer is doing okay
    glCheckError();
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
void Framebuffer::resize_framebuffer(uint32 _width, uint32 _height)
{
    if (width == _width && height == _height) return;
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // delete old buffers
    if (color_buffer.has_value())
    {
        if (color_buffer_access == GL_TEXTURE_2D)    glDeleteTextures(1, &color_buffer.value());
        if (color_buffer_access == GL_RENDERBUFFER)  glDeleteRenderbuffers(1, &color_buffer.value());
        color_buffer = std::nullopt;
    }

    if (depth_stencil.has_value())
    {
        if (depth_stencil_access == GL_TEXTURE_2D)    glDeleteTextures(1, &depth_stencil.value());
        if (depth_stencil_access == GL_RENDERBUFFER)  glDeleteRenderbuffers(1, &depth_stencil.value());
        depth_stencil = std::nullopt;
    }

    glDeleteFramebuffers(1, &fbo);
    fbo = 0;

    // assign new size and create new buffers
    width = _width;
    height = _height;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    label_gl(GL_FRAMEBUFFER, fbo, rdg_label.c_str());
    glCheckError();

    init_color_buffer(color_buffer_access);
    init_depth_stencil(depth_stencil_access);

    check_complete();
}

// private Impl
struct Window::Impl
{
    bool valid = false;
    glm::ivec2 size{0,0};
    GLFWwindow* window = nullptr;
    GLFWmonitor* monitor = nullptr;

};

// device specific definitions
Window::Window(uint32 _width, uint32 _height, const char* _title, bool _fullscreen) : pImpl{ std::make_unique<Impl>() }
{
    // Init GLFW
    if(!glfwInit())
    {
        log(Fatal, "glfwInit failed");
        assert(false);
    }

    log(Info, "GLFW version {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!_title)
    {
        log(Fatal, "_title is a nullptr");
        assert(false);
    }

    pImpl->size.x = glm::max(_width, 800u);
    pImpl->size.y = glm::max(_height, 600u);
    pImpl->monitor = glfwGetPrimaryMonitor();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(error_callback);

    if (_fullscreen)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(pImpl->monitor);

        auto maxScreenWidth = mode->width;
        auto maxScreenHeight = mode->height;
        pImpl->window = glfwCreateWindow(maxScreenWidth, maxScreenHeight, _title, pImpl->monitor, NULL);

        if(pImpl->window)
            glfwSetInputMode(pImpl->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
        pImpl->window = glfwCreateWindow(pImpl->size.x, pImpl->size.y, _title, NULL, NULL);

    if (!pImpl->window)
    {
        log(Fatal, "window creation failed");
        glfwTerminate();
        assert(false);
    }
    pImpl->valid = true;

#ifdef INDIGO_EDITOR
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif // INDIGO_EDITOR

    //set up additional window attribs
    glfwMakeContextCurrent(pImpl->window);
    glfwSetFramebufferSizeCallback(pImpl->window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    int width, height;
    glfwGetFramebufferSize(pImpl->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

    int major = glfwGetWindowAttrib(pImpl->window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(pImpl->window, GLFW_CONTEXT_VERSION_MINOR);
    int revision = glfwGetWindowAttrib(pImpl->window, GLFW_CONTEXT_REVISION);
    log(Info, "GLFW OpenGL context version {}.{}.{}", major, minor, revision);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log(Fatal, "GLAD failed to initialize OpenGL context");
        assert(false);
    }

    const auto* const vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const auto* const renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const auto* const version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const auto* const shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    log(Info, "Initialized GLFW");
    log(Info, "OpenGL Vendor {}", vendor);
    log(Info, "OpenGL Renderer {}", renderer);
    log(Info, "OpenGL Version {}", version);
    log(Info, "OpenGL Shader Version {}", shaderVersion);
    
    //install input mananger
    input.init(pImpl->window);

}
Window::~Window()
{ 
    if (pImpl->valid)
        glfwDestroyWindow(pImpl->window);

    glfwTerminate();
}

void* Window::get_window() const
{
    return pImpl->window;
}

const glm::ivec2& Window::get_window_size() const 
{
    glfwGetFramebufferSize(pImpl->window, &pImpl->size.x, &pImpl->size.y);
    return pImpl->size;
}

const float Window::get_monitor_ui_scale() const
{
    float xscale = 1.0f, yscale = 1.0f;
    glfwGetMonitorContentScale(pImpl->monitor, &xscale, &yscale);
    return xscale;
}

bool Window::is_running() const 
{
    if (pImpl->valid)
        return !glfwWindowShouldClose(pImpl->window);
    return false; // should not be running if window isn't initialized
}

void Window::begin_frame()
{

}
void Window::end_frame()
{

    glfwSwapBuffers(pImpl->window);
    glfwPollEvents();
}