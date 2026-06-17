#include "precomp.h"


Image::Image(const Model& _model, int32 _index) : Image()
{
    if (_model.original_file->type == FileStream::Type::Wavefront)
    {
        path = _model.get_wavefront_material(_index).diffuse_texname;

        auto image_file = FileIO::open_file(_model.original_file->directory, FileStream::Type::Image, FileIO::get_base_dir(*_model.original_file) + path);
        load_with_data(image_file->image_stream, image_file->image.width, image_file->image.height, image_file->image.channels);

    }
    else if (_model.original_file->type == FileStream::Type::GLTF)
    {
        auto& image = _model.get_gltf_model().images[_index];
        path = image.uri;

        if (image.uri.empty())
        {
            if (image.bufferView >= 0)
            {
                GLubyte* data = nullptr;
                const auto& view = _model.get_gltf_model().bufferViews[image.bufferView];
                const auto& buffer = _model.get_gltf_model().buffers[view.buffer];
                const auto* ptr = &buffer.data.at(view.byteOffset);
                data = stbi_load_from_memory(ptr, (int)buffer.data.size(), &width, &height, &channels, 4);
                if (data)
                {
                    load_with_data(data, width, height, 4, true);
                    stbi_image_free(data);
                }
                else
                {
                    log(Error, "Image could not be loaded from a PNG file. \nImage: {} \nURI: {}", _model.get_path(), image.uri);
                }
            }
            else if (!image.image.empty())
            {
                width = image.width;
                height = image.height;
                channels = image.component;
                load_with_data((uint8*)image.image.data(), width, height, 4, true);
            }
        }
        else
        {
            auto uri = _model.get_path();
            const auto lastSlashIdx = uri.rfind("/");
            uri = uri.substr(0, lastSlashIdx + 1);
            uri += image.uri;

            auto image_file =std::move(FileIO::open_file(FileStream::Directory::None, FileStream::Type::Image, FileIO::get_base_dir(*_model.original_file) + path));
        
            if (image_file->image_stream)
            {
                load_with_data(image_file->image_stream, image_file->image.width, image_file->image.height, image_file->image.channels);
            }
            else
            {
                log(Error, "Image could not be loaded from a file. \nImage: {} \nURI: {}", _model.get_path(), image.uri);
            }
        }
    }
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

static Sampler::Filter tinygltf_to_filter(int _in)
{
    switch (_in)
    {
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
        return Sampler::Filter::Linear;
        break;
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
        return Sampler::Filter::Nearest;
        break;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        return Sampler::Filter::LinearMipmapLinear;
        break;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        return Sampler::Filter::LinearMipmapNearest;
        break;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        return Sampler::Filter::NearestMipmapLinear;
        break;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        return Sampler::Filter::NearestMipmapNearest;
        break;
    default:
        // for opt == -1
        return Sampler::Filter::Nearest;
        break;
    }
}
static Sampler::Wrap tinygltf_to_wrap(int _in)
{
    switch (_in)
    {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
        return Sampler::Wrap::ClampToEdge;
        break;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
        return Sampler::Wrap::MirroredRepeat;
        break;
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
        return Sampler::Wrap::Repeat;
        break;
    default:
        // for opt == -1
        return Sampler::Wrap::Repeat;
        break;
    }
}

Sampler::Sampler(const Model& _model, int32 _index) : Sampler(Filter::Linear, Filter::Linear, Wrap::Repeat, Wrap::Repeat)
{
    if (_model.original_file->type == FileStream::Type::Wavefront)
    {
        //TODO: :o
    }
    else if (_model.original_file->type == FileStream::Type::GLTF)
    {
        mag_filter = tinygltf_to_filter(_model.get_gltf_model().samplers[_index].magFilter);
        min_filter = tinygltf_to_filter(_model.get_gltf_model().samplers[_index].minFilter);
        wrap_s = tinygltf_to_wrap(_model.get_gltf_model().samplers[_index].wrapS);
        wrap_t = tinygltf_to_wrap(_model.get_gltf_model().samplers[_index].wrapT);
    }
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
    if (_model.original_file->type == FileStream::Type::Wavefront)
    {
        if (_index >= _model.images.size()) return;
        if (_index >= _model.samplers.size()) return;

        image = _model.get_image(_index);
        sampler = _model.get_sampler(_index);
    }
    else if (_model.original_file->type == FileStream::Type::GLTF)
    {
        
        assert(_model.get_gltf_model().textures[_index].source != -1);
        image = _model.images[_model.get_gltf_model().textures[_index].source];

        if (_model.get_gltf_model().textures[_index].sampler != -1)
        {
            sampler = _model.samplers[_model.get_gltf_model().textures[_index].sampler];
        }
        else
        {
            sampler = std::make_shared<Sampler>();
        }
    }
}

Texture::Texture(std::shared_ptr<Image> _image, std::shared_ptr<Sampler> _sampler) : Texture()
{
    image = _image;
    sampler = _sampler;

}
