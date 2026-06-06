#include "precomp.h"

// callbacks
static void error_callback(int error, const char* description)
{
    tanlog::log(tanlog::ERROR, "glfw msg: %s", description);
}
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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

        tanlog::log(tanlog::ERROR, "{} | {} ({})", error, file, line);
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

std::string GetBaseDir(const std::string& filepath)
{
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

bool FileExists(const std::string& abs_filename)
{
    bool ret;
    FILE* fp;
    fopen_s(&fp, abs_filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    }
    else {
        ret = false;
    }

    return ret;
}

void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3])
{
    float v10[3];
    v10[0] = v1[0] - v0[0];
    v10[1] = v1[1] - v0[1];
    v10[2] = v1[2] - v0[2];

    float v20[3];
    v20[0] = v2[0] - v0[0];
    v20[1] = v2[1] - v0[1];
    v20[2] = v2[2] - v0[2];

    N[0] = v10[1] * v20[2] - v10[2] * v20[1];
    N[1] = v10[2] * v20[0] - v10[0] * v20[2];
    N[2] = v10[0] * v20[1] - v10[1] * v20[0];

    float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
    if (len2 > 0.0f) {
        float len = sqrtf(len2);

        N[0] /= len;
        N[1] /= len;
        N[2] /= len;
    }
}

bool LoadObjAndConvert(float bmin[3], float bmax[3],
    std::vector<Mesh>* meshes,
    std::vector<tinyobj::material_t>& materials,
    std::map<std::string, GLuint>& textures,
    const char* filename)
{
    tinyobj::attrib_t inattrib;
    std::vector<tinyobj::shape_t> inshapes;

    std::string base_dir = GetBaseDir(filename);
    if (base_dir.empty()) {
        base_dir = ".";
    }
#ifdef _WIN32
    base_dir += "\\";
#else
    base_dir += "/";
#endif

    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&inattrib, &inshapes, &materials, &warn, &err, filename,
        base_dir.c_str());
    if (!warn.empty()) {
        tanlog::log(tanlog::WARNING, warn);
    }
    if (!err.empty()) {
        tanlog::log(tanlog::ERROR, err);
    }

    if (!ret) {
        tanlog::log(tanlog::ERROR, filename);
        return false;
    }

    tanlog::log(tanlog::INFO, "# of vertices  = {}", (int)(inattrib.vertices.size()) / 3);
    tanlog::log(tanlog::INFO, "# of normals  = {}", (int)(inattrib.normals.size()) / 3);
    tanlog::log(tanlog::INFO, "# of texcoords  = {}", (int)(inattrib.texcoords.size()) / 2);
    tanlog::log(tanlog::INFO, "# of materials  = {}", (int)(materials.size()));
    tanlog::log(tanlog::INFO, "# of shapes  = {}", (int)(inshapes.size()));

    // Append `default` material
    materials.push_back(tinyobj::material_t());

    for (size_t i = 0; i < materials.size(); i++) {
        tanlog::log(tanlog::INFO, "material[{}].diffuse_texname = {}",
            int(i), materials[i].diffuse_texname.c_str());
    }

    // Load diffuse textures
    {
        for (size_t m = 0; m < materials.size(); m++) {
            tinyobj::material_t* mp = &materials[m];

            if (mp->diffuse_texname.length() > 0) {
                // Only load the texture if it is not already loaded
                if (textures.find(mp->diffuse_texname) == textures.end()) {
                    GLuint texture_id;
                    int w, h;
                    int comp;

                    std::string texture_filename = mp->diffuse_texname;
                    if (!FileExists(texture_filename)) {
                        // Append base dir.
                        texture_filename = base_dir + mp->diffuse_texname;
                        if (!FileExists(texture_filename)) {

                            tanlog::log(tanlog::FATAL, "Unable to find file: {}", mp->diffuse_texname);
                        }
                    }

                    unsigned char* image =
                        stbi_load(texture_filename.c_str(), &w, &h, &comp, STBI_default);
                    if (!image) {
                        tanlog::log(tanlog::FATAL, "Unable to load texture: {}", texture_filename);
                    }
                    tanlog::log(tanlog::INFO, "Loaded texture: {}, w = {}, h = {}, comp = {}", texture_filename, w, h, comp);

                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    if (comp == 3) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                            GL_UNSIGNED_BYTE, image);
                    }
                    else if (comp == 4) {
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                            GL_UNSIGNED_BYTE, image);
                    }
                    else {
                        assert(0);  // TODO
                    }
                    glBindTexture(GL_TEXTURE_2D, 0);
                    label_gl(GL_TEXTURE, texture_id, mp->diffuse_texname);
                    stbi_image_free(image);
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                }
            }
        }
    }

    bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
    bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

    std::vector<tinyobj::shape_t>& shapes = inshapes;
    tinyobj::attrib_t& attrib = inattrib;

    {
        for (size_t s = 0; s < shapes.size(); s++) {
            std::vector<float> buffer_pos;  // pos(3float)
            std::vector<float> buffer_norm;  // normal(3float)
            std::vector<float> buffer_color;  // color(3float)
            std::vector<float> buffer_texcoord;  // texcoord(2float)
            int _material_id = -1;

            std::vector<uint32> indices;

            for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
                tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

                int current_material_id = shapes[s].mesh.material_ids[f];
                _material_id = current_material_id;

                float diffuse[3];
                for (size_t i = 0; i < 3; i++) {
                    diffuse[i] = materials[current_material_id].diffuse[i];
                }
                float tc[3][2];
                if (attrib.texcoords.size() > 0) {
                    if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
                        (idx2.texcoord_index < 0)) {
                        // face does not contain valid uv index.
                        tc[0][0] = 0.0f;
                        tc[0][1] = 0.0f;
                        tc[1][0] = 0.0f;
                        tc[1][1] = 0.0f;
                        tc[2][0] = 0.0f;
                        tc[2][1] = 0.0f;
                    }
                    else {
                        assert(attrib.texcoords.size() >
                            size_t(2 * idx0.texcoord_index + 1));
                        assert(attrib.texcoords.size() >
                            size_t(2 * idx1.texcoord_index + 1));
                        assert(attrib.texcoords.size() >
                            size_t(2 * idx2.texcoord_index + 1));

                        // Flip Y coord.
                        tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                        tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
                        tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                        tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
                        tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                        tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
                    }
                }
                else {
                    tc[0][0] = 0.0f;
                    tc[0][1] = 0.0f;
                    tc[1][0] = 0.0f;
                    tc[1][1] = 0.0f;
                    tc[2][0] = 0.0f;
                    tc[2][1] = 0.0f;
                }

                float v[3][3];
                for (int k = 0; k < 3; k++) {
                    int f0 = idx0.vertex_index;
                    int f1 = idx1.vertex_index;
                    int f2 = idx2.vertex_index;
                    assert(f0 >= 0);
                    assert(f1 >= 0);
                    assert(f2 >= 0);

                    v[0][k] = attrib.vertices[3 * f0 + k];
                    v[1][k] = attrib.vertices[3 * f1 + k];
                    v[2][k] = attrib.vertices[3 * f2 + k];
                    bmin[k] = std::min(v[0][k], bmin[k]);
                    bmin[k] = std::min(v[1][k], bmin[k]);
                    bmin[k] = std::min(v[2][k], bmin[k]);
                    bmax[k] = std::max(v[0][k], bmax[k]);
                    bmax[k] = std::max(v[1][k], bmax[k]);
                    bmax[k] = std::max(v[2][k], bmax[k]);
                }

                float n[3][3];
                {
                    bool invalid_normal_index = false;
                    if (attrib.normals.size() > 0) {
                        int nf0 = idx0.normal_index;
                        int nf1 = idx1.normal_index;
                        int nf2 = idx2.normal_index;

                        if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
                            // normal index is missing from this face.
                            invalid_normal_index = true;
                        }
                        else {
                            for (int k = 0; k < 3; k++) {
                                assert(size_t(3 * nf0 + k) < attrib.normals.size());
                                assert(size_t(3 * nf1 + k) < attrib.normals.size());
                                assert(size_t(3 * nf2 + k) < attrib.normals.size());
                                n[0][k] = attrib.normals[3 * nf0 + k];
                                n[1][k] = attrib.normals[3 * nf1 + k];
                                n[2][k] = attrib.normals[3 * nf2 + k];
                            }
                        }
                    }
                    else {
                        invalid_normal_index = true;
                    }

                    if (invalid_normal_index) {
                        // compute geometric normal
                        CalcNormal(n[0], v[0], v[1], v[2]);
                        n[1][0] = n[0][0];
                        n[1][1] = n[0][1];
                        n[1][2] = n[0][2];
                        n[2][0] = n[0][0];
                        n[2][1] = n[0][1];
                        n[2][2] = n[0][2];
                    }
                }

                for (int k = 0; k < 3; k++) {
                    buffer_pos.push_back(v[k][0]);
                    buffer_pos.push_back(v[k][1]);
                    buffer_pos.push_back(v[k][2]);
                    buffer_norm.push_back(n[k][0]);
                    buffer_norm.push_back(n[k][1]);
                    buffer_norm.push_back(n[k][2]);
                    // Combine normal and diffuse to get color.
                    float normal_factor = 0.2f;
                    float diffuse_factor = 1 - normal_factor;
                    float c[3] = { n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                                  n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                                  n[k][2] * normal_factor + diffuse[2] * diffuse_factor };
                    float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                    if (len2 > 0.0f) {
                        float len = sqrtf(len2);

                        c[0] /= len;
                        c[1] /= len;
                        c[2] /= len;
                    }
                    buffer_color.push_back(c[0] * 0.5f + 0.5f);
                    buffer_color.push_back(c[1] * 0.5f + 0.5f);
                    buffer_color.push_back(c[2] * 0.5f + 0.5f);

                    buffer_texcoord.push_back(tc[k][0]);
                    buffer_texcoord.push_back(tc[k][1]);

                    indices.push_back((uint32)f);
                }
            }

            if (buffer_pos.size() > 0) {
                Mesh emp;
                emp.create_mesh(indices.data(), (uint32)indices.size(), buffer_pos.data(), buffer_norm.data(), buffer_texcoord.data(), buffer_color.data(), (uint32)buffer_pos.size() / 3);;
                emp.material_id = _material_id;
                meshes->push_back(emp);
            }
        }
    }
    tanlog::log(tanlog::INFO, "bmin = {}, {}, {}", bmin[0], bmin[1], bmin[2]);
    tanlog::log(tanlog::INFO, "bmax = {}, {}, {}", bmax[0], bmax[1], bmax[2]);

    return true;
}

Framebuffer::Framebuffer(const char* _rdg_label) : rdg_label{"BoringBuffer_>:("}
{
    glGenFramebuffers(1, &fbo);

    width = engine.get_window().get_window_size().x;
    height = engine.get_window().get_window_size().y;

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
    if(color_buffer.has_value())
    {
        tanlog::log(tanlog::ERROR, "trying to init_texturebuffer twice");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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
    if (depth_stencil.has_value())
    {
        tanlog::log(tanlog::ERROR, "trying to init_depth_stencil twice");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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

// private Impl
struct Window::Impl
{
    bool valid = false;
    glm::ivec2 size{0,0};
    GLFWwindow* window = nullptr;
};

// device specific definitions
Window::Window(uint32 _width, uint32 _height, const char* _title, bool _fullscreen) : pImpl{ std::make_unique<Impl>() }
{
    // Init GLFW
    if(!glfwInit())
    {
        tanlog::log(tanlog::FATAL, "glfwInit failed");
        assert(false);
    }

    tanlog::log(tanlog::INFO, "GLFW version {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    if (!_title)
    {
        tanlog::log(tanlog::FATAL, "_title is a nullptr");
        assert(false);
    }

    pImpl->size.x = glm::max(_width, 800u);
    pImpl->size.y = glm::max(_height, 600u);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(error_callback);

    if (_fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        auto maxScreenWidth = mode->width;
        auto maxScreenHeight = mode->height;
        pImpl->window = glfwCreateWindow(maxScreenWidth, maxScreenHeight, _title, monitor, NULL);

        if(pImpl->window)
            glfwSetInputMode(pImpl->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
        pImpl->window = glfwCreateWindow(pImpl->size.x, pImpl->size.y, _title, NULL, NULL);

    if (!pImpl->window)
    {
        tanlog::log(tanlog::FATAL, "window creation failed");
        glfwTerminate();
        assert(false);
    }
    pImpl->valid = true;

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
    tanlog::log(tanlog::INFO, "GLFW OpenGL context version {}.{}.{}", major, minor, revision);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        tanlog::log(tanlog::FATAL, "GLAD failed to initialize OpenGL context");
        assert(false);
    }

    const auto* const vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const auto* const renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const auto* const version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const auto* const shaderVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    tanlog::log(tanlog::INFO, "Initialized GLFW");
    tanlog::log(tanlog::INFO, "OpenGL Vendor {}", vendor);
    tanlog::log(tanlog::INFO, "OpenGL Renderer {}", renderer);
    tanlog::log(tanlog::INFO, "OpenGL Version {}", version);
    tanlog::log(tanlog::INFO, "OpenGL Shader Version {}", shaderVersion);
    
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(pImpl->window, false);
    ImGui_ImplOpenGL3_Init("#version 130");

    //install input mananger
    input.init(pImpl->window);

    ImGui_ImplGlfw_InstallCallbacks(pImpl->window);
}
Window::~Window()
{ 
    if (pImpl->valid)
        glfwDestroyWindow(pImpl->window);

    glfwTerminate();
}

const glm::ivec2& Window::get_window_size() const 
{
    glfwGetFramebufferSize(pImpl->window, &pImpl->size.x, &pImpl->size.y);
    return pImpl->size;
}

bool Window::is_running() const 
{
    if (pImpl->valid)
        return !glfwWindowShouldClose(pImpl->window);
    return false; // should not be running if window isn't initialized
}

void Window::begin_frame()
{
#ifdef INDIGO_EDITOR
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame(); 
    ImGuizmo::SetRect(0, 0, (float)pImpl->size.x, (float)pImpl->size.y);
#endif

}
void Window::end_frame()
{
#ifdef INDIGO_EDITOR
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif

    glfwSwapBuffers(pImpl->window);
    glfwPollEvents();
}