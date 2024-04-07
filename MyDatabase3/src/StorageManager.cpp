#include "StorageManager.h"
#include "RecordBuilder.h"
#include "DBexceptions.h"
#include "FileManager.h"


void InMemoryStorageManager::BuildStorage(const Schema& schema)
{
	DropAll();

	for (const Table& table : schema.GetTables())
		m_Store[table.GetName()] = StorageEngine(table);
}

void InMemoryStorageManager::Commit() 
{
}

void InMemoryStorageManager::DropAll()
{
	m_Store.clear();
}

StorageManager::StorageManager() { }

StorageManager::~StorageManager() { }

Schema StorageManager::BuildStore(const std::string& dbName)
{
	m_Store.clear();

	FileManager* fmanager = FileManager::GetInstance();
	fmanager->SetDatabaseRootPath("db", dbName);

	if (!fmanager->CheckDBExists())
		throw StorageManagerException("Database does not exits! " + dbName);

	Schema schema = fmanager->LoadDBSchema();
	fmanager->BuildDBFolderStructure(schema);
	_BuildTableFolderStructure(schema);

	return schema;
}

void StorageManager::BuildStore(const Schema& schema)
{
	m_Store.clear();

	FileManager* fmanager = FileManager::GetInstance();
	fmanager->SetDatabaseRootPath("db", schema.GetName());

	try
	{
		Schema savedSchema = fmanager->LoadDBSchema();
		if (!savedSchema.Match(schema))
			throw StorageEngineException("Given schema doesn't match with the saved schema!");
	}
	catch (const FileManagerExcpetion& e)
	{
		fmanager->BuildDBFolderStructure(schema);
		fmanager->SaveDBSchema(schema);
	}
	_BuildTableFolderStructure(schema);
}

void StorageManager::_BuildTableFolderStructure(const Schema& schema)
{
	FileManager* fmanager = FileManager::GetInstance();

	for (const Table& table : schema.GetTables())
	{
		StorageEngine& engine = m_Store[table.GetName()] = StorageEngine(table);
		EngineHeader& eHeader = engine.GetHeader();
		try
		{
			fmanager->ReadBinaryFile(table.GetName(), table.GetName() + ".header", (char*)(&eHeader), sizeof(EngineHeader));
		}
		catch (const FileManagerFileDoesNotExistsException& e)
		{
			std::cout << "No engine header: " << table.GetName() << ".header.dat \t Table not been commited yet!" << std::endl;
		}
	}
}

void StorageManager::Commit()
{
	FileManager* fmanager = FileManager::GetInstance();

	for (const auto& [name, engine] : m_Store)
	{
		const EngineHeader& engineHeader = engine.GetHeader();
		fmanager->WriteBinaryFile(name, name + ".header", (const char*)(&engineHeader), sizeof(EngineHeader));

		for (const Page& page : engine.GetPages())
		{
			const PageHeader& pageHeader = page.GetHeader();
			fmanager->WriteBinaryFile(name, name + "." + std::to_string(pageHeader.PageIndex) + ".header", (const char*)(&pageHeader), sizeof(PageHeader));
			fmanager->WritePageData(name, pageHeader.PageIndex, page.GetPage(), pageHeader.NumBytes);
		}
	}
}

void StorageManager::DropAll()
{
	FileManager* fmanager = FileManager::GetInstance();
	fmanager->DeleteDiatabase();
}

void StorageManager::Insert(const Table& table, const Record& record)
{
	StorageEngine& engine = m_Store[table.GetName()];

	RecordBuilder builder(table, record, -1);
	Record newRecord = builder.GetRecord();


	engine.AddRecord(newRecord);
	Commit();
}

std::vector<Record> StorageManager::Select(const Table& table)
{
	StorageEngine& engine = m_Store[table.GetName()];

	return engine.GetRecords(table);
}

Record StorageManager::Select(const Table& table, int id)
{
	StorageEngine& engine = m_Store[table.GetName()];

	try
	{
		return engine.GetRecord(table, id);
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Update(const Table& table, const Record& record, int id) 
{
	StorageEngine& engine = m_Store[table.GetName()];

	try
	{
		RecordBuilder builder(table, record, id);
		Record newRecord = builder.GetRecord();

		engine.Update(newRecord, id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Update(const Table& table, const Record& record)
{
	StorageEngine& engine = m_Store[table.GetName()];

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

		engine.Update(record, id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}

void StorageManager::Delete(const Table& table, int id)
{
	StorageEngine& engine = m_Store[table.GetName()];

	try
	{
		engine.DeleteRecord(id);
		Commit();
	}
	catch (const StorageEnginePageIndexOutOfBoundException& e)
	{
		throw StorageManagerException("Record does not exists with id: " + std::to_string(id));
	}
}