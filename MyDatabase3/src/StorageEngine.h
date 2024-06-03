#pragma once
#include "Record.h"
#include "Table.h"
#include "DBexceptions.h"
#include <deque>
#include <vector>
#include "Page.h"
#include <functional>


#define PAGE_SIZE 4096
#define MAX_NUM_PAGES 16


class StorageEnginePageIndexOutOfBoundException : public StorageEngineException
{
public:
	StorageEnginePageIndexOutOfBoundException() : StorageEngineException("Index out of bound from the page size.") { }
};


struct EngineHeader
{
	int NumPages = 0;
	int FreeRecordIndex = 0;
};


class StorageEngine
{
public:
	StorageEngine(int bytesPerRecord, const std::shared_ptr<IPageFactory>& pageFactory, const EngineHeader& engineHeader);
	virtual ~StorageEngine() = default;

	/*
		Always put the record in the first empty spot (fill the database sequentially) and sets the id to the spot's index.
		Set the first flag byte to 1 and pupulate the first record 4byte with the id(int).
	*/
	virtual void AddRecord(const Record& record);

	/*
		Gets the Record by id if it's exits. If not throw "StorageEnginePageIndexOutOfBoundException".
	*/
	virtual Record GetRecord(const Table& table, int id);

	/*
		Returns all the Records from the storage.
	*/
	virtual std::vector<Record> GetRecords(const Table& table);
	virtual std::vector<Record> GetRecords(const Table& table, const std::function<bool(const char*)>& condition);

	/*
		Expects a valid record with id as the first attribute for compatibility reasons.
		Overwrites the existing record at index == id with the inputed record.
		The newly updated record will inherite the previous id.
	*/
	virtual void Update(const Record& record, int id);

	/*
		Deletes the index == id record.
	*/
	virtual void DeleteRecord(int id);

	virtual int GetBytesPerRow() const { return m_BytesPerRow; }
	virtual int GetBytesPerRecord() const { return m_BytesPerRecord; }
	virtual int GetBytesPerPage() const { return m_PageFactory->GetByterPerPage(); }

	virtual const EngineHeader& GetHeader() const { return m_Header; }
	virtual EngineHeader& GetHeader() { return m_Header; }

	virtual std::shared_ptr<IPageFactory>& GetPageFactory() { return m_PageFactory; }
	virtual std::vector<std::unique_ptr<IPage>>& GetPages() { return m_Pages; }

protected:
	virtual char* GetRecordAt(int index);
	virtual char* FindRecordById(int id);

	virtual std::unique_ptr<IPage>& LoadPage(int index) = 0;


private:
	EngineHeader m_Header;
	int m_BytesPerRecord;
	int m_BytesPerRow;  // bytesPerRow - bytesPerRecord = flagBytes.

	std::shared_ptr<IPageFactory> m_PageFactory;
	std::vector<std::unique_ptr<IPage>> m_Pages;
};


class InMemoryStorageEngine : public StorageEngine
{
public:
	InMemoryStorageEngine(int bytesPerRecord, const std::shared_ptr<IPageFactory>& pageFactory);

protected:
	virtual std::unique_ptr<IPage>& LoadPage(int index) override;
};


class RegularStorageEngine : public StorageEngine
{
public:
	RegularStorageEngine(int bytesPerRecord, const std::shared_ptr<IPageFactory>& pageFactory /*, */);

protected:
	virtual std::unique_ptr<IPage>& LoadPage(int index) override;

private:
};

