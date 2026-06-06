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
			if(_stream)
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