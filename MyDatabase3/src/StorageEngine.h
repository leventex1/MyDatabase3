#pragma once
#include "Record.h"
#include "Table.h"
#include "DBexceptions.h"
#include <deque>
#include <vector>
#include "Page.h"


#define PAGE_SIZE 4096
#define MAX_NUM_PAGES 16


class StorageEnginePageIndexOutOfBoundException : public StorageEngineException
{
public:
	StorageEnginePageIndexOutOfBoundException() : StorageEngineException("Index out of bound from the page size.") { }
};


class IStorageEngine
{
public:
	virtual ~IStorageEngine() = default;

	virtual void AddRecord(const Record& record) = 0;
	virtual Record GetRecord(const Table& table, int id) = 0;
	virtual std::vector<Record> GetRecords(const Table& table) = 0;
	virtual void Update(const Record& record, int id) = 0;
	virtual void DeleteRecord(int id) = 0;

	virtual void Save() = 0;
	virtual void Load() = 0;
};


class InMemoryStorageEngine : public IStorageEngine
{
public:
	InMemoryStorageEngine(const std::shared_ptr<IPageFactory> pageFactory);
	virtual ~InMemoryStorageEngine();

	virtual void AddRecord(const Record& record) override;
	virtual Record GetRecord(const Table& table, int id) override;
	virtual std::vector<Record> GetRecords(const Table& table) override;
	virtual void Update(const Record& record, int id) override;
	virtual void DeleteRecord(int id) override;

private:
	std::shared_ptr<IPageFactory> m_PageFactory;
	std::vector<std::unique_ptr<IPage>> m_Pages;
};


struct EngineHeader
{
	int NumPages = 0;
	int FreeRecordIndex = 0;
};

class StorageEngine
{
public:
	StorageEngine();
	StorageEngine(const Table& table);

	void InitPage();

	/*
		Assumes the record is the right record structure.
		Auto populates the first 4 bytes(int) as the id.
	*/
	void AddRecord(const Record& record);
	Record GetRecord(const Table& table, int id);
	std::vector<Record> GetRecords(const Table& table);
	void Update(const Record& record, int id);
	void DeleteRecord(int id);
	
	inline size_t GetPageBytes() const { return PAGE_SIZE; }

	inline const std::deque<Page>& GetPages() const { return m_Pages; }
	inline std::deque<Page>& GetPages() { return m_Pages; }

	inline const EngineHeader& GetHeader() const { return m_Header; }
	inline EngineHeader& GetHeader() { return m_Header; }

private:
	char* _GetRecordAt(int index);
	char* _FindRecordById(int id);

	Page& _LoadPage(int index);

private:
	EngineHeader m_Header;
	std::string m_TableName;

	int m_BytesPerRecord = 0;	
	int m_FlagBytesPerRecord = 1;  // +1 byte flag.

	std::deque<Page> m_Pages;
};