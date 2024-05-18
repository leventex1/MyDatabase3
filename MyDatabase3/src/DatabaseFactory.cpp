#include "DatabaseFactory.h"
#include "DBexceptions.h"


std::unique_ptr<Database> DatabaseFactory::GetDatabase() 
{
	if (!m_Database)
		throw DatabaseException("Database not built yet!");

	return std::move(m_Database); 
}


void DatabaseFactory::BuildDatabaseType(DatabaseType type, const Schema& schema)
{
	switch (type)
	{
		case DatabaseType::Regular:
		{
			throw DatabaseException("Regular Database not implemented yet!");
			break;
		}
		case DatabaseType::InMemory:
		{
			std::shared_ptr<StorageManager> storageManager = std::make_shared<InMemoryStorageManager>();
			storageManager->BuildStorage(schema);
			m_Database = std::make_unique<Database>(
				schema,
				storageManager
			);
			break;
		}
		default:
			break;
	}
}


void DatabaseFactory::BuildDatabaseFromDisk(DatabaseType type, const std::string& dbName)
{
	switch (type)
	{
		case DatabaseType::Regular:
		{
			throw DatabaseException("Regular Database not implemented yet!");
			break;
		}
		case DatabaseType::InMemory:
		{
			throw DatabaseException("Cannot create an In-memory database from disk!");
			break;
		}
		default:
			break;
	}
}