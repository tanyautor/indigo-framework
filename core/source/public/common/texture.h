#pragma once

struct Sampler : public Resource
{
    enum class Filter : int32
    {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapLinear
    };

    enum class Wrap : int32
    {
        Repeat,
        ClampToEdge,
        MirroredRepeat
    };

    Sampler() : Resource(Resource::ResourceType::Sampler) {}
    Sampler(const Model& _model, int32 _index);
    Sampler(Filter mag_filter, Filter min_filter, Wrap wrap_s, Wrap wrap_t);

    void apply_sampler();

    static std::string get_path(const Model& _model, const int32& _index)
    {
        return _model.get_path() + " | Sampler-" + std::to_string(_index);
    }

protected:

    // default value from gltf2.0 standard lol
    Filter mag_filter{ Filter::Nearest };
    Filter min_filter{ Filter::Nearest };

    Wrap wrap_s{ Wrap::ClampToEdge };
    Wrap wrap_t{ Wrap::ClampToEdge };

private: 
    friend struct Image;
};

struct Image : public EditorInterface, public Resource
{
    Image() : Resource(Resource::ResourceType::Image) {}
    Image(const Model& _model, int32 _index);
    Image(const FileStream::Directory& _dir, const std::string& _file);
    Image(uint8* _data, uint32 _width, uint32 _height, uint32 _compression, bool _gen_mipmaps = false, Sampler _sampler = Sampler());
    

    virtual const std::string& get_name() override { return path; };
    virtual void interface_component() override;

    static std::string get_path(const Model& _model, const int32& _index)
    {
        return _model.get_path() + " | Texture-" + std::to_string(_index);
    }
    static std::string get_path(const FileStream::Directory& _dir, const std::string& _file)
    {
        return FileIO::get_full_path(_dir, _file);
    }

    // color channels: 3 -> rbg, 4 -> rgba, etc. you get it
    uint32 texture{ 0 };

    int32 channels{ 3 };
    int32 width{ -1 };
    int32 height{ -1 };

protected:
    void load_with_data(uint8* _data, uint32 _width, uint32 _height, uint32 _compression, bool _gen_mipmaps = false);
};

struct Texture : public Resource
{
    Texture() : Resource(Resource::ResourceType::Texture) {}
    Texture(const Model& _model, int32 _index);
    Texture(std::shared_ptr<Image> _image, std::shared_ptr<Sampler> _sampler);
    static std::string get_path(const Model& _model, const int32& _index)
    {
        return _model.get_path() + " | Texture-" + std::to_string(_index);
    }
    static std::string get_path(std::shared_ptr<Image> _image, std::shared_ptr<Sampler> _sampler)
    {
        return _image->get_name() + " | Texture";
    }


    std::shared_ptr<Image> image;
    std::shared_ptr<Sampler> sampler;
};