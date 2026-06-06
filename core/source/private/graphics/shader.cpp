#include "precomp.h"

namespace tan_files
{

    const std::regex sIncludeRegex = std::regex("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	std::vector<std::string> _searchPaths;
	std::map<std::string, std::string> _cachedSources;


	static std::string get_parent_path(const std::string& _path)
	{    
        // Implementation base on:
        // http://stackoverflow.com/questions/28980386/how-to-get-file-name-from-a-whole-path-by-c

        std::string parent = "";
        std::string::size_type found = _path.find_last_of("/");

        // if we found one of this symbols
        if (found != std::string::npos) parent = _path.substr(0, found);

        return parent;
	}

	static std::string parse_recursive(const std::string& _path, const std::string& _parent_path, std::set<std::string>& _includes)
	{

        //assuming _path is the full path for now
        std::string fullpath = _parent_path.empty() ? _path : _parent_path + "/" + _path;
        
        // check for circular includes
        if (_includes.count(fullpath))
        {
            tanlog::log(tanlog::ERROR, "found circular include in {}", _path);
            return std::string();
        }

        // include new file
        _includes.insert(fullpath);

        // get shader source
        std::string parent = get_parent_path(fullpath);
        std::string in_shader_src = read_txt_file(fullpath);
        if (in_shader_src.empty())
        {
            tanlog::log(tanlog::ERROR, "Shader file not found! Path: {}", fullpath);
            return std::string();
        }

        std::stringstream input(std::move(in_shader_src));
        std::stringstream parsed_src;


        std::string line;
        std::smatch matches;
        size_t lineNumber = 1;
        while (getline(input, line))
        {
            if (regex_search(line, matches, sIncludeRegex))
            {
                auto includeFile = parse_recursive(matches[1].str(), parent, _includes);
                parsed_src << includeFile;
                parsed_src << "#line " << lineNumber << '\n';
            }
            else
            {
                if (!line.empty() && line[0] != '\0')  // Don't null terminate
                    parsed_src << line;
            }
            parsed_src << '\n';
            lineNumber++;
        }

        return parsed_src.str();
	}

	std::string parse_shader(const std::string& _path)
	{
        std::set<std::string> includeTree;
		return parse_recursive(_path, "", includeTree);
	}

    std::string read_txt_file(const std::string& _path)
    {
        // TODO: cascading includes
        std::stringstream ret;

        std::ifstream shader_source;
        shader_source.open(_path);

        if (shader_source.is_open())
        {
            std::string line;
            while (std::getline(shader_source, line))
            {
                ret << "\n";
                ret << line;
            }
            shader_source.close();
        }

        return ret.str();
    }
}