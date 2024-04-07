#include "FileIO.h"

#include <fstream>
#include <sstream>


void IFileIO::_WriteFile_(std::ofstream& file, const std::string& content) const
{
	if (!file.is_open())
		throw FileIOException("Failed to open file for write!");

	file.write(content.c_str(), content.size());

	if (!file.good())
		throw FileIOException("Error occured at writing time!");

	file.close();
}

std::string IFileIO::_ReadFile_(std::ifstream& file) const
{
	if (!file.is_open())
		throw FileIOException("Failed to open file for read!");

	std::stringstream buffer;
	buffer << file.rdbuf();

	file.close();

	return buffer.str();
}

void BinaryFileIO::WriteFile(const std::string& path, const std::string& name, const std::string& content) const
{
	std::string filePath = path + "/" + name + ".dat";
	std::ofstream file(filePath, std::ios::binary);
	
	IFileIO::_WriteFile_(file, content);
}

std::string BinaryFileIO::ReadFile(const std::string& path, const std::string& name) const
{
	std::string filePath = path + "/" + name + ".dat";
	std::ifstream file(filePath, std::ios::binary);

	return IFileIO::_ReadFile_(file);
}


void TextFileIO::WriteFile(const std::string& path, const std::string& name, const std::string& content) const
{
	std::string filePath = path + "/" + name + ".txt";
	std::ofstream file(filePath);

	IFileIO::_WriteFile_(file, content);
}

std::string TextFileIO::ReadFile(const std::string& path, const std::string& name) const
{
	std::string filePath = path + "/" + name + ".txt";
	std::ifstream file(filePath);

	return IFileIO::_ReadFile_(file);
}