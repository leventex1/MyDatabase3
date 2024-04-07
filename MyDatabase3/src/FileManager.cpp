#include "FileManager.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "DBexceptions.h"


FileManager* FileManager::s_Instance = nullptr;

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

FileManager* FileManager::GetInstance()
{
	if (!s_Instance)
		s_Instance = new FileManager();

	return s_Instance;
}

void FileManager::SetDatabaseRootPath(const std::string& rootPath, const std::string& dbName)
{
	m_DatabaseRootPath = rootPath + "/" + dbName;
}

bool FileManager::CheckDBExists()
{
	return std::filesystem::exists(m_DatabaseRootPath) && std::filesystem::is_directory(m_DatabaseRootPath);
}

void FileManager::BuildDBFolderStructure(const Schema& schema)
{
	std::cout << "Building folder structure at: " << m_DatabaseRootPath << std::endl;

	const std::vector<Table>& tables = schema.GetTables();
	for (const Table& table : tables)
	{
		std::string path = m_DatabaseRootPath + "/" + table.GetName();
		if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
		{
			std::cout << path << " already exists!" << std::endl;
		}
		else
		{
			if (std::filesystem::create_directories(path))
			{
				std::cout << path << " (*)" << std::endl;
			}
			else
			{
				throw FileManagerExcpetion("Failed to create path at: " + path);
			}
		}
	}
}

void FileManager::SaveDBSchema(const Schema& schema) const
{
	std::cout << "Writing schema file! (*)" << std::endl;

	std::string schemaStr = schema.ToString();

	std::ofstream file(m_DatabaseRootPath + "/schema.txt");

	if (!file.is_open())
	{
		throw FileManagerExcpetion("Failed to open schema file!");
	}

	file << schemaStr;

	if (!file.good())
	{
		throw FileManagerExcpetion("Error occured at writing time!");
	}

	file.close();
}

Schema FileManager::LoadDBSchema() const
{
	std::cout << "Loading schema file!" << std::endl;

	std::ifstream file(m_DatabaseRootPath + "/schema.txt");

	if (!file.is_open())
	{
		throw FileManagerExcpetion("Failed to open schema file!");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	Schema schema("");
	schema.FromString(buffer.str());

	return schema;
}

void FileManager::WriteBinaryFile(const std::string& tableName, const std::string& fileName, const char* data, size_t numBytes) const
{
	std::string filePath = m_DatabaseRootPath + "/" + tableName + "/" + fileName + ".dat";
	std::ofstream file(filePath, std::ios::binary);

	if (!file.is_open())
	{
		throw FileManagerExcpetion("Failed to open file! " + filePath);
	}

	file.write(data, numBytes);

	if (!file.good())
	{
		throw FileManagerExcpetion("Error occured at writing time!");
	}

	file.close();
}

void FileManager::ReadBinaryFile(const std::string& tableName, const std::string& fileName, char* data, size_t numBytes) const
{
	std::string filePath = m_DatabaseRootPath + "/" + tableName + "/" + fileName + ".dat";
	if (!std::filesystem::exists(filePath) || std::filesystem::is_directory(filePath))
	{
		throw FileManagerFileDoesNotExistsException();
	}

	std::ifstream file(filePath, std::ios::binary);


	if (!file.is_open())
	{
		throw FileManagerExcpetion("Failed to open file! " + filePath);
	}

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize > numBytes)
	{
		throw FileManagerExcpetion("File size if bigger than the buffer size! " + std::to_string(fileSize) + ">" + std::to_string(numBytes));
	}

	file.read(data, fileSize);

	if (!file.good())
	{
		throw FileManagerExcpetion("Error occured at reading time!");
	}

	file.close();
}

void FileManager::WritePageData(const std::string& tableName, int index, const char* data, size_t numBytes) const
{
	std::string fileName = tableName + "." + std::to_string(index);
	WriteBinaryFile(tableName, fileName, data, numBytes);
}

void FileManager::ReadPageData(const std::string& tableName, int index, char* data, size_t numBytes) const
{
	std::string fileName = tableName + "." + std::to_string(index);
	ReadBinaryFile(tableName, fileName, data, numBytes);
}

void FileManager::DeleteDiatabase() const
{
	std::cout << "Deleting database! " << m_DatabaseRootPath << std::endl;
	std::uintmax_t numRemoved = std::filesystem::remove_all(m_DatabaseRootPath);
	if (numRemoved > 0) {
		std::cout << "Database directory and its contents deleted successfully, items removed: " << numRemoved << std::endl;
	}
	else {
		std::cout << "Database directory deletion failed or directory does not exist." << std::endl;
	}
}