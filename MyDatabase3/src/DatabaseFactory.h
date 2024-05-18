#pragma once
#include "Database.h"


enum class DatabaseType
{
	Regular = 0, InMemory
};



class DatabaseFactory
{
public:
	DatabaseFactory() = default;

	void BuildDatabaseType(DatabaseType type, const Schema& schema);
	void BuildDatabaseFromDisk(DatabaseType type, const std::string& dbName);

	std::unique_ptr<Database> GetDatabase();

protected:
	std::unique_ptr<Database> m_Database;
};
