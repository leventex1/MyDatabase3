#include "StorageManager.h"
#include "RecordBuilder.h"
#include "DBexceptions.h"
#include "FileManager.h"
#include "Serializer.h"


std::unique_ptr<StorageEngine>& StorageManager::GetStorageEngine(const std::string& tableName)
{
	std::unique_ptr<StorageEngine>& engine = m_Store[tableName];

	if (!engine)
		throw StorageManagerException("Storage engine not found! (" + tableName + ") Probably StorageManager->BuildStorage(Schema) haven't been called!");

	return engine;
}

void StorageManager::Insert(const Table& table, const Record& record)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	RecordBuilder builder(table, record, -1);
	Record newRecord = builder.GetRecord();

	engine->AddRecord(newRecord);
	Commit();
}

std::vector<Record> StorageManager::Select(const Table& table)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	return engine->GetRecords(table);
}

std::vector<Record> StorageManager::Select(const Table& table, const std::function<bool(const char*)>& condition)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	return engine->GetRecords(table, condition);
}

Record StorageManager::Select(const Table& table, int id)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	try
	{
		return engine->GetRecord(table, id);
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Update(const Table& table, const Record& record, int id) 
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	try
	{
		RecordBuilder builder(table, record, id);
		Record newRecord = builder.GetRecord();

		engine->Update(newRecord, id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Update(const Table& table, const Record& record)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	int id = -1;
	try
	{
		const RecordAttr& idAttr = record.GetAttrs()[0];
		try
		{
			id = std::stoi(idAttr.Value);
		}
		catch (...)
		{
			throw InsertionException("Can't convert id value to int! (" + idAttr.Value + ")");
		}

		engine->Update(record, id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Delete(const Table& table, int id)
{
	std::unique_ptr<StorageEngine>& engine = GetStorageEngine(table.GetName());

	try
	{
		engine->DeleteRecord(id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}


void InMemoryStorageManager::BuildStorage(const Schema& schema)
{
	DropAll();
	for (const Table& table : schema.GetTables())
		m_Store[table.GetName()] = std::make_unique<InMemoryStorageEngine>(table.GetBytes(), std::make_shared<DefaultPageFactory>(4096));
}

void InMemoryStorageManager::Commit() 
{
}

void InMemoryStorageManager::DropAll()
{
	m_Store.clear();
}


RegularStorageManager::RegularStorageManager(const std::shared_ptr<FileManager>& fileManager)
	: m_FileManager(fileManager)
{
}

void RegularStorageManager::BuildStorage(const Schema& schema)
{
	m_Store.clear();

	StorageEngineHeaderSerializer headerSerializer(1);

	for (const Table& table : schema.GetTables())
	{
		auto& engine = m_Store[table.GetName()] = std::make_unique<RegularStorageEngine>(table.GetBytes(), std::make_shared<DefaultPageFactory>(4096));

		try
		{
			std::string engineHeaderContent = m_FileManager->ReadHeader(table.GetName());
			EngineHeader engineHeader = headerSerializer.Deserialize(engineHeaderContent);
			engine->GetHeader() = engineHeader;
		}
		catch (const FileIOException& e) 
		{ 
			std::string engineHeaderContent = headerSerializer.Serialize(engine->GetHeader());
			m_FileManager->WriteHeader(table.GetName(), engineHeaderContent);
		}

	}
}

void RegularStorageManager::Commit()
{
}

void RegularStorageManager::DropAll()
{
	m_Store.clear();
}