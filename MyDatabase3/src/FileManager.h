#pragma once
#include <string>
#include "Schema.h"
#include "DBexceptions.h"


class FileManagerFileDoesNotExistsException : public DatabaseException
{
public:
	FileManagerFileDoesNotExistsException() : DatabaseException("File does not exits!") { }
};


class FileManager
{
private:
	FileManager();
	~FileManager();
public:
	static FileManager* GetInstance();

	void SetDatabaseRootPath(const std::string& rootPath, const std::string& dbName);

	bool CheckDBExists();

	void BuildDBFolderStructure(const Schema& schema);
	void SaveDBSchema(const Schema& schema) const;
	Schema LoadDBSchema() const;

	void WriteBinaryFile(const std::string& tableName, const std::string& fileName, const char* data, size_t numBytes) const;
	void ReadBinaryFile(const std::string& tableName, const std::string& fileName, char* data, size_t numBytes) const;

	void WritePageData(const std::string& tableName, int index, const char* data, size_t numBytes) const;
	void ReadPageData(const std::string& tableName, int index, char* data, size_t numBytes) const;

	void DeleteDiatabase() const;

private:
	static FileManager* s_Instance;

	std::string m_DatabaseRootPath;
};
