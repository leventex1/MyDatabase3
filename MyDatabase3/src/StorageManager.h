#pragma once
#include "Schema.h"
#include "Record.h"
#include <map>
#include <string>
#include <vector>
#include "StorageEngine.h"
#include <functional>
#include "FileManager.h"


class StorageManager
{
public:
	StorageManager() = default;
	StorageManager(std::map<std::string, std::unique_ptr<StorageEngine>>& store) : m_Store(std::move(store)) { }
	virtual ~StorageManager() = default;

	virtual void BuildStorage(const Schema& schema) = 0;
	virtual void Commit() = 0;
	virtual void DropAll() = 0;

	/*
		Inserts a record into the database. The record must not contain the Id attribute.
	*/
	virtual void Insert(const Table& table, const Record& record);
	
	/*
		Assumes table exists in store.
		Select every record from the store.
	*/
	virtual std::vector<Record> Select(const Table& table);
	virtual std::vector<Record> Select(const Table& table, const std::function<bool(const char*)>& condition);
	
	/*
		Assumes table exists in store.
		Select record at id from the store.
	*/
	virtual Record Select(const Table& table, int id);
	
	/*
		Record is a raw record, it does not contain the id attr (first attr).
	*/
	virtual void Update(const Table& table, const Record& record, int id);

	/*
		Record contain the id attr (first attr). If not exists throw "StorageManagerException"
	*/
	virtual void Update(const Table& table, const Record& record);

	/*
		Delete the index == id row from the database if exits. If not throw "StorageManagerException"
	*/
	void Delete(const Table& table, int id);

private:
	std::unique_ptr<StorageEngine>& GetStorageEngine(const std::string& tableName);

protected:
	std::map<std::string, std::unique_ptr<StorageEngine>> m_Store;
};


class InMemoryStorageManager : public StorageManager
{
public:
	InMemoryStorageManager() = default;
	virtual ~InMemoryStorageManager() = default;

	virtual void BuildStorage(const Schema& schema) override;
	virtual void Commit() override;
	virtual void DropAll() override;
};


class RegularStorageManager : public StorageManager
{
public:
	RegularStorageManager(const std::shared_ptr<FileManager>& fileManager);
	virtual ~RegularStorageManager() = default;

	virtual void BuildStorage(const Schema& schema) override;
	virtual void Commit() override;
	virtual void DropAll() override;

private:
	std::shared_ptr<FileManager> m_FileManager;
};