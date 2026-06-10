#include "precomp.h"



FileStream FileIO::open_file(FileStream::Directory _global_dir, FileStream::Type _type, std::string _file)
{
	FileStream file;
	file.type = _type;
	file.directory = _global_dir;
	file.local_path = _file;

	std::fstream stream;
	std::string full_path = get_full_path(file);

	// open file stream
	switch (_type)
	{
	case FileStream::Type::Binary:
		stream.open(full_path, stream.binary | stream.in | stream.out);
		if(stream.is_open())
		{
			log(Severity::ERROR, "Failed to open file {}", full_path);
		}
		break;
	case FileStream::Type::Text:
		stream.open(full_path, stream.in | stream.out);
		if (stream.is_open())
		{
			log(Severity::ERROR, "Failed to open file {}", full_path);
		}
		break;
	case FileStream::Type::Wavefront:
		break;
	case FileStream::Type::GLTF:
		break;
	default:
		break;
	}

	return file;
}

bool FileIO::file_exists(const FileStream& _file)
{
	std::fstream file;
	file.open(get_full_path(_file), file.binary | file.in);
	return file.is_open();
}

bool FileIO::file_exists(FileStream::Directory _global_dir, std::string _file)
{
	std::fstream file;
	file.open(get_full_path(_global_dir, _file), file.binary | file.in);
	return file.is_open();
}

bool FileIO::file_exists(std::string _fullpath)
{
	std::fstream file;
	file.open((_fullpath), file.binary | file.in);
	return file.is_open();
}
