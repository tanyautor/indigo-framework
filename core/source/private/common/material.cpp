#include "precomp.h"

Material::Material(const Model& _model, int32 _index) : Material()
{
	auto fileio = engine.get_file_io();

    auto base_dir = fileio->get_base_dir(*_model.original_file);
    std::string texture_name;

    // if loading a wavefront, create textures
    if (_model.original_file->type == FileStream::Type::Wavefront)
    {
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
                auto image = std::make_shared<Image>(FileStream::Directory::None, texture_filename);
                auto sampler = std::make_shared<Sampler>(_model, _index);
                base_color = std::make_shared<Texture>(image, sampler);

                num_registered_textures++;
            }
            else
            {
                log(Error, "Unable to find file: {}", texture_name);
            }
        }
        // normal texture
        {
            texture_name = mp.normal_texname;
            std::string texture_filename = base_dir + texture_name;
            if (!texture_name.empty() && fileio->file_exists(texture_filename))
            {
                // check if image already loaded in
                auto image = std::make_shared<Image>(FileStream::Directory::None, texture_filename);
                auto sampler = std::make_shared<Sampler>(_model, _index);
                normal_map = std::make_shared<Texture>(image, sampler);

                num_registered_textures++;
            }
            else
            {
                log(Error, "Unable to find file: {}", texture_name);
            }
        }
        // emissive texture
        {
            texture_name = mp.emissive_texname;
            std::string texture_filename = base_dir + texture_name;
            if (!texture_name.empty() && fileio->file_exists(texture_filename))
            {
                // check if image already loaded in
                auto image = std::make_shared<Image>(FileStream::Directory::None, texture_filename);
                auto sampler = std::make_shared<Sampler>(_model, _index);
                emissive = std::make_shared<Texture>(image, sampler);

                num_registered_textures++;
            }
            else
            {
                log(Error, "Unable to find file: {}", texture_name);
            }
        }
        // metallic texture
        {
            texture_name = mp.metallic_texname;
            std::string texture_filename = base_dir + texture_name;
            if (!texture_name.empty() && fileio->file_exists(texture_filename))
            {
                // check if image already loaded in
                auto image = std::make_shared<Image>(FileStream::Directory::None, texture_filename);
                auto sampler = std::make_shared<Sampler>(_model, _index);
                metallic_roughness = std::make_shared<Texture>(image, sampler);

                num_registered_textures++;
            }
            else
            {
                log(Error, "Unable to find file: {}", texture_name);
            }
        }

        // other material attributes
        vec4 base_color_factor = { mp.diffuse[0], mp.diffuse[1] ,mp.diffuse[2], 1.f };

        // Metallic Roughness
        float metallic_factor = mp.metallic;
        float roughness_factor = mp.roughness;

        // Normal
        float normal_scale = 1.f;

        // Occlusion
        float occlusion_strength = 0.f;

        // Emissive
        vec3 emissive_factor = { mp.emission[0], mp.emission[1] ,mp.emission[2] };
    }
    else if(_model.original_file->type == FileStream::Type::GLTF)
    {
        const auto& material = _model.get_gltf_model().materials[_index];

        emissive_factor = { material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2] };

        if (material.emissiveTexture.index != -1)
        {
            emissive = _model.textures[material.emissiveTexture.index];
        }

        if (material.normalTexture.index != -1)
        {
            normal_map = _model.textures[material.normalTexture.index];
            normal_scale = (float)material.normalTexture.scale;
        }

        if (material.occlusionTexture.index != -1)
        {
            occlusion = _model.textures[material.occlusionTexture.index];
            occlusion_strength = (float)material.occlusionTexture.strength;
        }

        {
            const auto& pbr = material.pbrMetallicRoughness;
            base_color_factor = { pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3] };
            if (pbr.baseColorTexture.index != -1)
            {
                base_color = _model.textures[pbr.baseColorTexture.index];
            }

            if (pbr.metallicRoughnessTexture.index != -1)
            {
                metallic_roughness = _model.textures[pbr.metallicRoughnessTexture.index];
            }

            metallic_factor = (float)pbr.metallicFactor;
            roughness_factor = (float)pbr.roughnessFactor;
        }
    }



}
