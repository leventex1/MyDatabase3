#include "FileManager.h"
#include <filesystem>


FileManager::FileManager(const std::string& rootPath, const Schema& schema, std::unique_ptr<IFileIO>& fileIO)
	: m_RootPath(rootPath + "/" + schema.GetName()), m_FileIO(std::move(fileIO))
{
	std::filesystem::create_directory(m_RootPath);

	TextFileIO txtIO;
	txtIO.WriteFile(m_RootPath, schema.GetName(), schema.ToString());

	for (const Table& table : schema.GetTables())
	{
		std::string dirPath = m_RootPath + "/" + table.GetName();
		std::filesystem::create_directory(dirPath);
	}
}

std::string FileManager::ReadPage(const std::string& tableName, int pageIndex) const
{
	return m_FileIO->ReadFile(m_RootPath + "/" + tableName, tableName + std::to_string(pageIndex));
}

void FileManager::WritePage(const std::string& tableName, int pageIndex, const std::string& content) const
{
	m_FileIO->WriteFile(m_RootPath + "/" + tableName, tableName + std::to_string(pageIndex), content);
}

std::string FileManager::ReadHeader(const std::string& tableName) const
{
	return m_FileIO->ReadFile(m_RootPath + "/" + tableName, tableName + ".header");
}

void FileManager::WriteHeader(const std::string& tableName, const std::string& content) const
{
	m_FileIO->WriteFile(m_RootPath + "/" + tableName, tableName + ".header", content);
}
