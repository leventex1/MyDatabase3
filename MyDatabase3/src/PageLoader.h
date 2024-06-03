#pragma once
#include "Serializer.h"
#include "FileManager.h"


class PageLoader
{
public:
	PageLoader(const std::shared_ptr<FileManager>& fileManager, const std::string& tableName)
		: m_FileManager(fileManager), m_TableName(tableName) { }


private:
	const std::shared_ptr<FileManager> m_FileManager;
	std::string m_TableName;
};