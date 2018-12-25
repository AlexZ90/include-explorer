#include <string>

std::string getFileName(const std::string& filename)
{
	size_t start_pos = filename.find_last_of("/") + 1;
	return filename.substr(start_pos, filename.length()-start_pos);
}

std::string getFileExtension(const std::string& filename)
{
	size_t start_pos = filename.find_last_of(".") + 1;
	return filename.substr(start_pos, filename.length()-start_pos);
}
