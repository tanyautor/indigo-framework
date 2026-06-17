#include "precomp.h"

Material::Material(const Model& _model, int32 _index) : Material()
{
	auto fileio = engine.get_file_io();
	auto res = engine.get_resource_manager();

    auto base_dir = fileio->get_base_dir(_model.original_file);
    std::string texture_name;

    // Create Textures
    // taking some short cuts here because wavefront is a little backwards imo
    tinyobj::material_t mp = _model.get_wavefront_material(_index);
    path = mp.name;

    // diffuse texture
    {
        texture_name = mp.diffuse_texname;
        std::string texture_filename = base_dir + texture_name;
        if (!texture_name.empty() && fileio->file_exists(texture_filename))
        {
            // check if image already loaded in
            auto image = res->load_resource<Image>(FileStream::Directory::None, texture_filename);
            auto sampler = res->load_resource<Sampler>(_model, _index);
            base_color = res->load_resource<Texture>(image, sampler);

            num_registered_textures++;
        }
        else
        {
            log(ERROR, "Unable to find file: {}", texture_name);
        }
    }
    // normal texture
    {
        texture_name = mp.normal_texname;
        std::string texture_filename = base_dir + texture_name;
        if (!texture_name.empty() && fileio->file_exists(texture_filename))
        {
            // check if image already loaded in
            auto image = res->load_resource<Image>(FileStream::Directory::None, texture_filename);
            auto sampler = res->load_resource<Sampler>(_model, _index);
            normal_map = res->load_resource<Texture>(image, sampler);

            num_registered_textures++;
        }
        else
        {
            log(ERROR, "Unable to find file: {}", texture_name);
        }
    }
    // emissive texture
    {
        texture_name = mp.emissive_texname;
        std::string texture_filename = base_dir + texture_name;
        if (!texture_name.empty() && fileio->file_exists(texture_filename))
        {
            // check if image already loaded in
            auto image = res->load_resource<Image>(FileStream::Directory::None, texture_filename);
            auto sampler = res->load_resource<Sampler>(_model, _index);
            emissive = res->load_resource<Texture>(image, sampler);

            num_registered_textures++;
        }
        else
        {
            log(ERROR, "Unable to find file: {}", texture_name);
        }
    }
    // metallic texture
    {
        texture_name = mp.metallic_texname;
        std::string texture_filename = base_dir + texture_name;
        if (!texture_name.empty() && fileio->file_exists(texture_filename))
        {
            // check if image already loaded in
            auto image = res->load_resource<Image>(FileStream::Directory::None, texture_filename);
            auto sampler = res->load_resource<Sampler>(_model, _index);
            metallic_roughness = res->load_resource<Texture>(image, sampler);

            num_registered_textures++;
        }
        else
        {
            log(ERROR, "Unable to find file: {}", texture_name);
        }
    }

    // other material attributes
    vec4 base_color_factor = { mp.diffuse[0], mp.diffuse[1] ,mp.diffuse[2], 1.f};

    // Metallic Roughness
    float metallic_factor = mp.metallic;
    float roughness_factor = mp.roughness;

    // Normal
    float normal_scale = 1.f;

    // Occlusion
    float occlusion_strength = 0.f;

    // Emissive
    vec3 emissive_factor = { mp.emission[0], mp.emission[1] ,mp.emission[2]};
}
