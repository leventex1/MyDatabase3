#pragma once
#include <string>
#include "FileIO.h"
#include "Schema.h"


class FileManager
{
public:
	FileManager(const std::string& rootPath, const Schema& schema, std::unique_ptr<IFileIO>& fileIO);
	virtual ~FileManager() = default;

	virtual std::string ReadPage(const std::string& tableName, int pageIndex) const;
	virtual void WritePage(const std::string& tableName, int pageIndex, const std::string& content) const;

	virtual std::string ReadHeader(const std::string& tableName) const;
	virtual void WriteHeader(const std::string& tableName, const std::string& content) const;

private:
	std::string m_RootPath;
	std::unique_ptr<IFileIO> m_FileIO;
};