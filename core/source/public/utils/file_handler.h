#pragma once


// there is no safety on any of this... so this is a huge TODO
namespace FileHandler
{
	static std::string saved_path{ "saved/" };
	static std::string shared_path{ "shared/" };
	static std::string bin_file_ext{ ".bin" };

	template<typename T>
	static bool save_bin(std::string _file, T* _data, uint32 _size)
	{
		if (!_data) return false;

		std::fstream f;
		std::string file_path{ saved_path + _file + bin_file_ext };
		f.open(file_path, std::fstream::out | std::fstream::binary);

		if (f.is_open())
		{
			f.write(reinterpret_cast<char*>(_data), _size);
			f.close();

			return true;
		}
		else
			log(indigo_log::Error, "failed to write to saved bin: {}", file_path.c_str());

		return false;
	}

	// this will overwrite _data, no matter if its already valid, so beware
	template<typename T>
	static bool load_bin(std::string _file, T* _data, uint32 _size)
	{
		if (!_data) return false;

		std::fstream f;
		std::string file_path{ saved_path + _file + bin_file_ext };
		f.open(file_path, std::fstream::in | std::fstream::binary);

		if (f.is_open())
		{
			f.read(reinterpret_cast<char*>(_data), _size);
			f.close();

			return true;
		}
		else
			log(indigo_log::Error, "failed to find/open saved bin file: {}", file_path.c_str());

		return false;
	}

	namespace Image
	{
		static void load_image_stream(std::string _file_in_shared, unsigned char* _stream, int32& _compression, uint32& _width, uint32& _height)
		{
			// want to pass ownership to outside the function
			if (_stream) return;

			std::string file_path{ shared_path + _file_in_shared };
			_stream = stbi_load(file_path.c_str(), reinterpret_cast<int*>(&_width), reinterpret_cast<int*>(&_height), &_compression, 0);
			if (_stream)
			{
				log(indigo_log::Info, "loaded image file {} with compression {}", file_path, _compression);
			}
			else
			{
				log(indigo_log::Error, "failed to load image {}", file_path);
				_compression = 0;
				_height = 0;
				_width = 0;
				return;
			}
		}
	}
}

struct FileStream
{
	~FileStream()
	{
		// most of this is integers, don't care if they go out of scope
		// but clean up open file stream
		if (stream)
		{
			free(stream);
		}
	}

	enum class Type : int32 { Binary, Text, Wavefront, GLTF, Image, } type{ Type::Text };
	enum class Directory : int32 { None, Assets, Shared, Saved, } directory{ Directory::Shared };

	std::string local_path{};

	// no const correctness, for some crackhead energy :/
	void* stream{ nullptr }; // base stream

	// keeping the upcasted version, so we don't have to reinterpret_cast on every access
	std::fstream* file_stream{ nullptr };
	uint8* image_stream{ nullptr };

	struct ImageData
	{
		int32 width{ -1 }, height{ -1 }, channels{ -1 };
	} image;
};

static std::string none_path		{ "" };
static std::string asset_path		{ "assets/" };
static std::string shared_path		{ "shared/" };
static std::string saved_path		{ "saved/" };

class FileIO
{
public:

	static const std::string& get_directory(FileStream::Directory _global_dir)
	{
		switch (_global_dir)
		{
		case FileStream::Directory::None:
			return none_path;
			break;
		case FileStream::Directory::Assets:
			return asset_path;
			break;
		case FileStream::Directory::Shared:
			return shared_path;
			break;
		case FileStream::Directory::Saved:
			return saved_path;
			break;
		default:
			log(indigo_log::Warning, "Failed to find global directory {}", magic_enum::enum_name(_global_dir));
			return none_path;
			break;
		}

	}
	static const std::string get_full_path(const FileStream& _stream)
	{
		return get_directory(_stream.directory) + _stream.local_path;
	}
	static const std::string get_full_path(const FileStream::Directory& _dir, const std::string& _file)
	{
		return get_directory(_dir) + _file;
	}
	static const std::string get_file_ext(const FileStream& _stream)
	{
		// TODO: no safety for invalid _stream
		return _stream.local_path.substr(_stream.local_path.find_last_of(".") + 1);
	}
	static const std::string get_file_ext(const std::string& _file)
	{
		// TODO: no safety for invalid _stream
		return _file.substr(_file.find_last_of("."));
	}
	static const std::string get_base_dir(const FileStream& _stream)
	{
		std::string full_path = get_full_path(_stream);
		if (full_path.find_last_of("/\\") != std::string::npos)
		{
			std::string base_dir = full_path.substr(0, full_path.find_last_of("/\\"));
			base_dir += "/";
			return base_dir;
		}
		return "";
	}

	[[nodiscard]] static std::unique_ptr<FileStream> open_file(FileStream::Directory _global_dir = FileStream::Directory::Shared, FileStream::Type _type = FileStream::Type::Text, std::string _file = "")
	{
		auto file{ std::make_unique<FileStream>() };
		file->type = _type;
		file->directory = _global_dir;
		file->local_path = _file;
		std::string full_path = get_full_path(*file);

		if (file->stream)
		{
			free(file->stream);
			file->stream = nullptr;
			file->file_stream = nullptr;
			file->image_stream = nullptr;
		}

		// open file stream
		switch (_type)
		{
		case FileStream::Type::Binary:
			// allocate stream with malloc/free, we can use this to free stbi image data too
			file->stream = malloc(sizeof(std::fstream));

			file->file_stream = reinterpret_cast<std::fstream*>(file->stream);
			file->file_stream->open(full_path, std::fstream::binary | std::fstream::in | std::fstream::out);
			if (!file->file_stream->is_open())
			{
				log(indigo_log::Error, "Failed to open file {}", full_path);
			}
			break;
		case FileStream::Type::Text:
			// allocate stream with malloc/free, we can use this to free stbi image data too
			file->stream = malloc(sizeof(std::fstream));

			file->file_stream = reinterpret_cast<std::fstream*>(file->stream);
			file->file_stream->open(full_path, std::fstream::in | std::fstream::out);
			if (!file->file_stream->is_open())
			{
				log(indigo_log::Error, "Failed to open file {}", full_path);
			}
			break;
		case FileStream::Type::Wavefront:
			break;
		case FileStream::Type::GLTF:
			break;
		case FileStream::Type::Image:
			file->stream = stbi_load(full_path.c_str(), &file->image.width, &file->image.height, &file->image.channels, 4);
			file->image_stream = reinterpret_cast<uint8*>(file->stream);
			break;
		default:
			break;
		}

		return std::move(file);
	}


	static bool file_exists(const FileStream& _file)
	{
		std::fstream file;
		file.open(get_full_path(_file), file.binary | file.in);
		return file.is_open();
	}
	static bool file_exists(FileStream::Directory _global_dir = FileStream::Directory::Shared, std::string _file = "")
	{
		std::fstream file;
		file.open(get_full_path(_global_dir, _file), file.binary | file.in);
		return file.is_open();
	}
	static bool file_exists(std::string _fullpath = "")
	{
		std::fstream file;
		file.open((_fullpath), file.binary | file.in);
		return file.is_open();
	}

};
