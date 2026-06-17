#include "precomp.h"


using shader = uint32;
using sh_vert = uint32;
using sh_frag = uint32;
using sh_comp = uint32;
constexpr int32 shader_dbg_msg_len{ 512 };

// private Impl
struct Shader::Impl
{
    ShaderType type{ ShaderType::SH_NONE };
    shader id;

    bool parse_shader(const uint32 _sh, const char* _src)
    {
        glShaderSource(_sh, 1, &_src, NULL);
        glCompileShader(_sh); 

        // handle errors
        int32 success;
        glGetShaderiv(_sh, GL_COMPILE_STATUS, &success);

        return success;
    }
};

// device specific definitions
Shader::Shader() : pImpl{ std::make_unique<Impl>() }
{
}
Shader::Shader(const Shader& other)
{
    pImpl.reset(new Impl(*other.pImpl));
}
Shader::Shader(std::string _vertex_path, std::string _fragment_path) : Shader()
{
    std::string vertex_source = indigo_files::parse_shader(_vertex_path);
    std::string fragment_source = indigo_files::parse_shader(_fragment_path);

    init_fragment(vertex_source, fragment_source);
    if (!is_valid())
        log(Error, "error thrown compiling files: \n{}\n{}", _vertex_path, _fragment_path);

    label_gl(GL_PROGRAM, pImpl->id, _fragment_path.c_str());
}
Shader::Shader(std::string _compute_path) : Shader()
{
    std::string compute_source = indigo_files::parse_shader(_compute_path);

    init_compute(compute_source);
    if (!is_valid())
        log(Error, "error thrown compiling file: \n{}", _compute_path);

    label_gl(GL_PROGRAM, pImpl->id, _compute_path.c_str());
}

Shader::~Shader()
{
    if(pImpl && pImpl->id)
        glDeleteProgram(pImpl->id);
}

Shader& Shader::operator=(const Shader& other)
{
    pImpl.reset(new Impl(*other.pImpl));
    return *this;
}

Shader& Shader::operator=( Shader&& other) noexcept
{
    pImpl = std::move(other.pImpl);
    return *this;
}

void Shader::init_fragment(std::string _vertex_src, std::string _fragment_src)
{
    if (is_valid())
    {
        log(Warning, "cannot create shader twice, shader is already valid...");
        return;
    }
    sh_vert vertex{ 0 };
    sh_frag fragment{ 0 };

    shader program{ 0 };

    // parse vert & frag shaders
    vertex = glCreateShader(GL_VERTEX_SHADER);
    if (!pImpl->parse_shader(vertex, _vertex_src.c_str()))
    {
        char infoLog[shader_dbg_msg_len];
        glGetShaderInfoLog(vertex, shader_dbg_msg_len, NULL, infoLog);
        log(Error, "vertex shader compilation failed: \n{}", infoLog);
        log(Error, "shader source: \n{}", _vertex_src.c_str());
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if (!pImpl->parse_shader(fragment, _fragment_src.c_str()))
    {
        char infoLog[shader_dbg_msg_len];
        glGetShaderInfoLog(vertex, shader_dbg_msg_len, NULL, infoLog);
        log(Error, "fragment shader compilation failed: \n{}", infoLog);
        log(Error, "shader source: \n{}", _fragment_src.c_str());
    }

    // link & create program
    program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    // handle errors
    int32 success;
    char infoLog[shader_dbg_msg_len];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        log(Error, "shader program failed to link: \n{}", infoLog);

        glDeleteProgram(program);
    }
    else
    {
        // Succeeded
        pImpl->id = program;
        pImpl->type = ShaderType::SH_FRAGMENT;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return;
}
void Shader::init_compute(std::string _compute_src)
{
    if (is_valid())
    {
        log(Warning, "cannot create shader twice, shader is already valid...");
        return;
    }

    sh_comp compute{ 0 };
    
    shader program{ 0 };
    
    compute = glCreateShader(GL_COMPUTE_SHADER);
    if (!pImpl->parse_shader(compute, _compute_src.c_str()))
    {
        char infoLog[shader_dbg_msg_len];
        glGetShaderInfoLog(compute, shader_dbg_msg_len, NULL, infoLog);
        log(Error, "compute shader compilation failed: \n{}", infoLog);
        log(Error, "shader source: \n{}", _compute_src.c_str());
    }

    // link & create program
    program = glCreateProgram();

    glAttachShader(program, compute);
    glLinkProgram(program);

    // handle errors
    int32 success;
    char infoLog[shader_dbg_msg_len];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        log(Error, "shader program failed to link: \n{}", infoLog);

        glDeleteProgram(program);
    }
    else
    {
        // Succeeded
        pImpl->id = program;
        pImpl->type = ShaderType::SH_COMPUTE;
    }

    glDeleteShader(compute);
}

bool Shader::is_valid() const { return pImpl->id; }
const uint32 Shader::id() const
{
    return pImpl->id;
}
void Shader::use_shader() const { glUseProgram(pImpl->id); }

