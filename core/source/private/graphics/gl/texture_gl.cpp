#include "precomp.h"


Image::Image(const Model& _model, int32 _index) : Image()
{
    path = _model.get_wavefront_material(_index).diffuse_texname;
    std::string texture_file = engine.get_file_io()->get_base_dir(_model.original_file) + path;

    int w, h;
    int comp;
    unsigned char* image_data =
        stbi_load(texture_file.c_str(), &w, &h, &comp, STBI_default);

    load_with_data(image_data, w, h, comp);
}

Image::Image(const FileStream::Directory& _dir, const std::string& _file) : Image()
{
    std::string path = FileIO::get_full_path(_dir, _file);

    int w, h;
    int comp;
    unsigned char* image_data =
        stbi_load(path.c_str(), &w, &h, &comp, STBI_default);

    load_with_data(image_data, w, h, comp);
    
    stbi_image_free(image_data);

    label_gl(GL_TEXTURE, texture, path);
}

Image::Image(uint8* _data, uint32 _width, uint32 _height, uint32 _channels, bool _gen_mipmaps, Sampler _sampler) : Image()
{
    load_with_data(_data, _width, _height, _channels, _gen_mipmaps);

    label_gl(GL_TEXTURE, texture, path);
}

void Image::interface_component()
{
}

void Image::load_with_data(uint8* _data, uint32 _width, uint32 _height, uint32 _channels, bool _gen_mipmaps)
{
    // This is a public method, so the fields
    // might undefined before this call
    GLint format = GL_INVALID_VALUE;
    GLint usage = GL_INVALID_VALUE;
    width = _width;
    height = _height;
    channels = _channels;
    switch (channels)
    {
    case 1:
        format = GL_R8;
        usage = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        usage = GL_RGB;
        break;
    case 4:
        format = GL_RGBA; // TODO: keep an eye on this
        usage = GL_RGBA;
        break;
    default:
        assert(false);
    }

    if (!texture) glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);  // Bind

    glTexImage2D(GL_TEXTURE_2D,     // What (target)
        0,                          // Mip-map level
        format,                     // Internal format
        width,                      // Width
        height,                     // Height
        0,                          // Border
        usage,                      // Format (how to use)
        GL_UNSIGNED_BYTE,           // Type   (how to interpret)
        _data);                     // Data

    if (_gen_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    glCheckError();
}

Sampler::Sampler(const Model& _model, int32 _index) : Sampler(Filter::Linear, Filter::Linear, Wrap::Repeat, Wrap::Repeat)
{
}

Sampler::Sampler(Filter _mag_filter, Filter _min_filter, Wrap _wrap_s, Wrap _wrap_t) : Sampler()
{
    mag_filter = _mag_filter;
    min_filter = _min_filter;

    wrap_s = _wrap_s;
    wrap_t = _wrap_t;

}

void Sampler::apply_sampler()
{
    int32 mag = GL_INVALID_ENUM, min = GL_INVALID_ENUM;
    switch (mag_filter)
    {
    case Sampler::Filter::Nearest:
        mag = GL_NEAREST;
        break;
    case Sampler::Filter::Linear:
        mag = GL_LINEAR;
        break;
    case Sampler::Filter::NearestMipmapNearest:
        mag = GL_NEAREST_MIPMAP_NEAREST;
        break;
    case Sampler::Filter::LinearMipmapNearest:
        mag = GL_LINEAR_MIPMAP_NEAREST;
        break;
    case Sampler::Filter::NearestMipmapLinear:
        mag = GL_NEAREST_MIPMAP_LINEAR;
        break;
    case Sampler::Filter::LinearMipmapLinear:
        mag = GL_LINEAR_MIPMAP_LINEAR;
        break;
    default:
        break;
    }
    switch (min_filter)
    {
    case Sampler::Filter::Nearest:
        min = GL_NEAREST;
        break;
    case Sampler::Filter::Linear:
        min = GL_LINEAR;
        break;
    case Sampler::Filter::NearestMipmapNearest:
        min = GL_NEAREST_MIPMAP_NEAREST;
        break;
    case Sampler::Filter::LinearMipmapNearest:
        min = GL_LINEAR_MIPMAP_NEAREST;
        break;
    case Sampler::Filter::NearestMipmapLinear:
        min = GL_NEAREST_MIPMAP_LINEAR;
        break;
    case Sampler::Filter::LinearMipmapLinear:
        min = GL_LINEAR_MIPMAP_LINEAR;
        break;
    default:
        break;
    }

    int32 s = GL_INVALID_ENUM, t = GL_INVALID_ENUM;
    switch (wrap_s)
    {
    case Sampler::Wrap::Repeat:
        s = GL_REPEAT;
        break;
    case Sampler::Wrap::ClampToEdge:
        s = GL_CLAMP_TO_EDGE;
        break;
    case Sampler::Wrap::MirroredRepeat:
        s = GL_MIRRORED_REPEAT;
        break;
    default:
        break;
    }
    switch (wrap_t)
    {
    case Sampler::Wrap::Repeat:
        t = GL_REPEAT;
        break;
    case Sampler::Wrap::ClampToEdge:
        t = GL_CLAMP_TO_EDGE;
        break;
    case Sampler::Wrap::MirroredRepeat:
        t = GL_MIRRORED_REPEAT;
        break;
    default:
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    glCheckError();
}

Texture::Texture(const Model& _model, int32 _index) : Texture()
{
    // load (not create) image and sampler resource from model and index

    if (_index >= _model.images.size()) return;
    if (_index >= _model.samplers.size()) return;

    image = _model.get_image(_index);
    sampler = _model.get_sampler(_index);

}

Texture::Texture(std::shared_ptr<Image> _image, std::shared_ptr<Sampler> _sampler) : Texture()
{
    image = _image;
    sampler = _sampler;

}
