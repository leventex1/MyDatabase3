#pragma once
#include "Schema.h"
#include "Record.h"
#include <map>
#include <string>
#include <vector>
#include "StorageEngine.h"


class IStorageManager
{
public:
	virtual ~IStorageManager() = default;

	virtual void BuildStorage(const Schema& schema) = 0;
	virtual void Commit() = 0;
	virtual void DropAll() = 0;
};


class InMemoryStorageManager : public IStorageManager
{
public:
	InMemoryStorageManager() = default;
	virtual ~InMemoryStorageManager() = default;

	virtual void BuildStorage(const Schema& schema) override;
	virtual void Commit() override;
	virtual void DropAll() override;

private:
	std::map<std::string, StorageEngine> m_Store;
};


class StorageManager
{
public:
	StorageManager();
	~StorageManager();

	/*
		Tires to build the database with the given db name from the folder structure.
	*/
	Schema BuildStore(const std::string& dbName);
	/*
		Assumes schema is valid.
	*/
	void BuildStore(const Schema& schema);

	/*
		Assumes table exists in store and record fits into the schema.
	*/
	void Insert(const Table& table, const Record& record);

	/*
		Assumes table exists in store.
		Select every record from the store.
	*/
	std::vector<Record> Select(const Table& table);

	/*
		Assumes table exists in store.
		Select record at id from the store.
	*/
	Record Select(const Table& table, int id);

	/*
		Record is a raw record, it does not contain the id attr (first attr).
	*/
	void Update(const Table& table, const Record& record, int id);
	/*
		Record contain the id attr (first attr).
	*/
	void Update(const Table& table, const Record& record);


	void Delete(const Table& table, int id);

	void Commit();
	void DropAll();

private:
	void _BuildTableFolderStructure(const Schema& schema);

private:
	std::map<std::string, StorageEngine> m_Store;
};
