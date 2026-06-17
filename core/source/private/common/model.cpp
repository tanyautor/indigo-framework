#include "precomp.h"

Model::Model(FileStream::Directory _dir, const std::string& _file) : Resource(ResourceType::Model)
{
	// TODO: hard coded to obj files only
	path = _file;
	transform.name = _file;

	// load file based on extension
	std::string extension = FileIO::get_file_ext(_file);
	if(extension == ".obj")
	{
		original_file = engine.get_file_io()->open_file(_dir, FileStream::Type::Wavefront, _file);
		create_from_obj(*original_file);
	}
	else if (extension == ".gltf" || extension == ".glb")
	{
		original_file = engine.get_file_io()->open_file(_dir, FileStream::Type::GLTF, _file);
		create_from_gltf(*original_file);
	}
	else
	{
		log(Error, "tried to load model with unknown extension: {}, full path: {}", extension, FileIO::get_full_path(_dir, _file));
	}
}

void Model::create_from_obj(const FileStream& _file)
{
	auto fileio = engine.get_file_io();
	auto resource_manager = engine.get_resource_manager();

	std::string base_dir = fileio->get_base_dir(_file);

	// local space 0-1
	vec3 bounds_min{ 0 };
	vec3 bounds_max{ 1 };

	// attributes, shapes, and materials loaded from wavefont
	tinyobj::attrib_t inattrib;

	// Load data into memory
	{
		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&inattrib, &wavefront_shapes, &wavefront_materials, &warn, &err, fileio->get_full_path(_file).c_str(), base_dir.c_str());
		if (!warn.empty())
		{
			log(Warning, warn);
		}
		if (!err.empty())
		{
			log(Error, err);
		}
		if (!ret) {
			log(Error, _file.local_path);
			return;
		}

		log(Info, "# of vertices  = {}", (int)(inattrib.vertices.size()) / 3);
		log(Info, "# of normals  = {}", (int)(inattrib.normals.size()) / 3);
		log(Info, "# of texcoords  = {}", (int)(inattrib.texcoords.size()) / 2);
		log(Info, "# of materials  = {}", (int)(wavefront_materials.size()));
		log(Info, "# of shapes  = {}", (int)(wavefront_shapes.size()));

		for (size_t i = 0; i < wavefront_materials.size(); i++) {
			log(Info, "material[{}].diffuse_texname = {}",
				int(i), wavefront_materials[i].diffuse_texname.c_str());
		}
	}

	// Materials & their Textures
	for (int32 m = 0; m < wavefront_materials.size(); m++)
	{
		tinyobj::material_t* mp = &wavefront_materials[m];

		auto material = resource_manager->load_resource<Material>(*this, m);

		// Create Textures & Samplers
		if (auto texture = material->base_color)
		{
			images.push_back(texture->image);
			samplers.push_back(texture->sampler);
		}
		if (auto texture = material->normal_map)
		{
			images.push_back(texture->image);
			samplers.push_back(texture->sampler);
		}
		if (auto texture = material->emissive)
		{
			images.push_back(texture->image);
			samplers.push_back(texture->sampler);
		}
		if (auto texture = material->metallic_roughness)
		{
			images.push_back(texture->image);
			samplers.push_back(texture->sampler);
		}
		if (auto texture = material->occlusion)
		{
			images.push_back(texture->image);
			samplers.push_back(texture->sampler);
		}

		materials.push_back(material);
	}

	// Meshes
	std::vector<tinyobj::shape_t>& shapes = wavefront_shapes;
	tinyobj::attrib_t& attrib = inattrib;

	for (int32 s = 0; s < shapes.size(); s++)
	{
		int _material_id = -1;

		// faces
		for (int32 f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
		{
			tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
			tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
			tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

			int current_material_id = shapes[s].mesh.material_ids[f];
			_material_id = current_material_id;

			auto current_mat = materials[current_material_id];
			vec3 diffuse{ current_mat->base_color_factor };

			// get texture coordinates
			vec2 texcoord[3] = {};
			if (attrib.texcoords.size() > 0)
			{
				if ((idx0.texcoord_index < 0) || 
					(idx1.texcoord_index < 0) ||
					(idx2.texcoord_index < 0)) 
				{
					// face does not contain valid uv index.
					texcoord[0] = { 0.f, 0.f };
					texcoord[1] = { 0.f, 0.f };
					texcoord[2] = { 0.f, 0.f };
				}
				else 
				{
					assert(attrib.texcoords.size() >
						size_t(2 * idx0.texcoord_index + 1));
					assert(attrib.texcoords.size() >
						size_t(2 * idx1.texcoord_index + 1));
					assert(attrib.texcoords.size() >
						size_t(2 * idx2.texcoord_index + 1));

					// Flip Y coord.
					texcoord[0] =
					{
								attrib.texcoords[2 * idx0.texcoord_index],
						1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1]
					};
					texcoord[1] =
					{
								attrib.texcoords[2 * idx1.texcoord_index],
						1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1]
					};
					texcoord[2] =
					{
								attrib.texcoords[2 * idx2.texcoord_index],
						1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1]
					};
				}
			}
			else 
			{
				texcoord[0] = { 0.f, 0.f };
				texcoord[1] = { 0.f, 0.f };
				texcoord[2] = { 0.f, 0.f };
			}

			// get vertex
			vec3 vertex[3] = {};
			for (int k = 0; k < 3; k++) 
			{
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;
				assert(f0 >= 0);
				assert(f1 >= 0);
				assert(f2 >= 0);

				vertex[0][k] = attrib.vertices[3 * f0 + k];
				vertex[1][k] = attrib.vertices[3 * f1 + k];
				vertex[2][k] = attrib.vertices[3 * f2 + k];
			}

			// get normal
			vec3 normal[3] = {};
			bool invalid_normal_index = false;
			if (attrib.normals.size() > 0) 
			{
				int nf0 = idx0.normal_index;
				int nf1 = idx1.normal_index;
				int nf2 = idx2.normal_index;

				if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) 
				{
					// normal index is missing from this face.
					invalid_normal_index = true;
				}
				else 
				{
					for (int k = 0; k < 3; k++) 
					{
						assert(size_t(3 * nf0 + k) < attrib.normals.size());
						assert(size_t(3 * nf1 + k) < attrib.normals.size());
						assert(size_t(3 * nf2 + k) < attrib.normals.size());
						normal[0][k] = attrib.normals[3 * nf0 + k];
						normal[1][k] = attrib.normals[3 * nf1 + k];
						normal[2][k] = attrib.normals[3 * nf2 + k];
					}
				}
			}

			// fill buffers
			for (int k = 0; k < 3; k++) 
			{
				wavefront_buffer_pos.push_back(vertex[k][0]);
				wavefront_buffer_pos.push_back(vertex[k][1]);
				wavefront_buffer_pos.push_back(vertex[k][2]);
				wavefront_buffer_norm.push_back(normal[k][0]);
				wavefront_buffer_norm.push_back(normal[k][1]);
				wavefront_buffer_norm.push_back(normal[k][2]);
				// Combine normal and diffuse to get color.
				float normal_factor = 0.2f;
				float diffuse_factor = 1 - normal_factor;
				float c[3] = {  normal[k][0] * normal_factor + diffuse[0] * diffuse_factor,
								normal[k][1] * normal_factor + diffuse[1] * diffuse_factor,
								normal[k][2] * normal_factor + diffuse[2] * diffuse_factor };
				float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
				if (len2 > 0.0f) 
				{
					float len = sqrtf(len2);

					c[0] /= len;
					c[1] /= len;
					c[2] /= len;
				}
				wavefront_buffer_color.push_back(c[0] * 0.5f + 0.5f);
				wavefront_buffer_color.push_back(c[1] * 0.5f + 0.5f);
				wavefront_buffer_color.push_back(c[2] * 0.5f + 0.5f);

				wavefront_buffer_texcoord.push_back(texcoord[k][0]);
				wavefront_buffer_texcoord.push_back(texcoord[k][1]);

				wavefront_indices.push_back((uint32)f);
			}
		}

		auto mesh = resource_manager->load_resource<Mesh>(*this, s);
		meshes.push_back(mesh);

		mesh->transform.set_parent(std::shared_ptr<Transform>(&transform));

		glCheckError();
	}

}

void Model::create_from_gltf(const FileStream& _file)
{
	tinygltf::TinyGLTF loader;
	std::string err, warn;
	std::string full_path = FileIO::get_full_path(_file);

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, full_path);

	if (!warn.empty())	log(Warning, "tinygltf warning: \n{}", warn);
	if (!err.empty())	log(Error, "tinygltf warning: \n{}", err);
	
	if (res)			log(Info, "loaded gltf: {}", _file.local_path);
	else				log(Error, "filed to load gltf: {}", _file.local_path);

	auto resource_manager = engine.get_resource_manager();

	// Load images (texture data)
	for (int i = 0; i < static_cast<int>(model.images.size()); i++)
	{
		auto image = resource_manager->load_resource<Image>(*this, i);
		images.push_back(image);
	}

	// Load samplers
	for (int i = 0; i < static_cast<int>(model.samplers.size()); i++)
	{
		auto sampler = std::make_shared<Sampler>(*this, i);
		samplers.push_back(sampler);
	}

	// Load textures
	for (int i = 0; i < static_cast<int>(model.textures.size()); i++)
	{
		auto texture = std::make_shared<Texture>(*this, i);
		textures.push_back(texture);
	}

	// Load materials
	for (int i = 0; i < static_cast<int>(model.materials.size()); i++)
	{
		auto material = resource_manager->load_resource<Material>(*this, i);
		materials.push_back(material);
	}	
	
	// Load meshes
	for (int i = 0; i < static_cast<int>(model.meshes.size()); i++)
	{
		auto mesh = resource_manager->load_resource<Mesh>(*this, i);
		meshes.push_back(mesh);
		mesh->transform.set_parent(std::shared_ptr<Transform>(&transform));
	}
}
