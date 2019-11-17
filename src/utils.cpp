#include <algorithm>
#include <string>
#include <iostream>
#include "utils.h"
#include "headers.h"


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

langtype getFileLang(const std::string& extension)
{
	if (extension.compare("c") == 0)
		return langtype::CLANG;
	if (extension.compare("S") == 0)
		return langtype::CLANG;
	if (extension.compare("cc") == 0)
		return langtype::CLANG;
	else if (extension.compare("cpp") == 0)
		return langtype::CXXLANG;
	else
	{
		std::cout << "Error: " << __FILE__ << " " << __LINE__ << " " << __func__ << ";" << std::endl;
		std::cout << "Error: Unknown file extension (" << extension << ").";
		std::cout << "Exit" << std::endl;
		exit (-1);
	}
}

bool checkLibForHeader(const std::string& filename, langtype parent_file_type)
{
	if (parent_file_type == langtype::CLANG)
	{
		std::string * header = std::find(std::begin(c_headers), std::end(c_headers), filename);
		if (header != std::end(c_headers)) {
			return true;
		} else {
			return false;
		}
	}
	else if (parent_file_type == langtype::CXXLANG)
	{
		std::string * header = std::find(std::begin(cxx_headers), std::end(cxx_headers), filename);
		if (header != std::end(cxx_headers)) {
			return true;
		} else {
			return false;
		}
	}
	else
	{
		std::cout << __func__ << " Unknown argument." << std::endl;
		return false;
	}
}
