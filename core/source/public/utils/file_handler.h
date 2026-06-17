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
			log(ERROR, "failed to write to saved bin: {}", file_path.c_str());

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
			log(ERROR, "failed to find/open saved bin file: {}", file_path.c_str());

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
				log(INFO, "loaded image file {} with compression {}", file_path, _compression);
			}
			else
			{
				log(ERROR, "failed to load image {}", file_path);
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
		if (stream && stream->is_open())
		{
			stream->close();
		}
	}

	enum class Type : int32 { Binary, Text, Wavefront, GLTF, };
	enum class Directory : int32 { None, Assets, Shared, Saved, };

	// stream will be unused if opened Wavefron or GLTF
	std::shared_ptr<std::fstream> stream{};
	
	Directory directory{ Directory::Shared };
	Type type{ Type::Text };
	std::string local_path{};
};

class FileIO
{
public:

	static std::string get_directory(FileStream::Directory _global_dir)
	{
		std::string directory_path{};

		switch (_global_dir)
		{
		case FileStream::Directory::None:
			directory_path = { "" };
			break;
		case FileStream::Directory::Assets:
			directory_path = { "assets/" };
			break;
		case FileStream::Directory::Shared:
			directory_path = { "shared/" };
			break;
		case FileStream::Directory::Saved:
			directory_path = { "saved/" };
			break;
		default:
			log(Severity::WARNING, "Failed to find global directory {}", magic_enum::enum_name(_global_dir));
			break;
		}

		return directory_path;
	}
	static std::string get_full_path(const FileStream& _stream)
	{
		return get_directory(_stream.directory) + _stream.local_path;
	}
	static std::string get_full_path(const FileStream::Directory& _dir, const std::string& _file)
	{
		return get_directory(_dir) + _file;
	}
	static std::string get_file_ext(const FileStream& _stream)
	{
		// TODO: no safety for invalid _stream
		return _stream.local_path.substr(_stream.local_path.find_last_of(".") + 1);
	}
	static std::string get_base_dir(const FileStream& _stream)
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

	[[nodiscard]] FileStream open_file(FileStream::Directory _global_dir = FileStream::Directory::Shared, FileStream::Type _type = FileStream::Type::Text, std::string _file = "");

	bool file_exists(const FileStream& _file);
	bool file_exists(FileStream::Directory _global_dir = FileStream::Directory::Shared, std::string _file = "");
	bool file_exists(std::string _fullpath = "");

private:

};
