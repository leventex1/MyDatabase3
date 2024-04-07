#include "StorageEngine.h"
#include "RecordBuilder.h"
#include "FileManager.h"


InMemoryStorageEngine::InMemoryStorageEngine(const std::shared_ptr<IPageFactory> pageFactory)
	: m_PageFactory(pageFactory)
{
}

InMemoryStorageEngine::~InMemoryStorageEngine()
{
}

void InMemoryStorageEngine::AddRecord(const Record& record)
{
}

Record InMemoryStorageEngine::GetRecord(const Table& table, int id)
{

	return Record();
}

std::vector<Record> InMemoryStorageEngine::GetRecords(const Table& table)
{
	return { };
}

void InMemoryStorageEngine::Update(const Record& record, int id)
{
}

void InMemoryStorageEngine::DeleteRecord(int id)
{
}


StorageEngine::StorageEngine()
	: m_TableName("")
{
	InitPage();
}

StorageEngine::StorageEngine(const Table& table)
	: m_BytesPerRecord(table.GetBytes()), m_TableName(table.GetName())
{
	InitPage();
}

void StorageEngine::InitPage()
{
	if (m_BytesPerRecord + m_FlagBytesPerRecord > PAGE_SIZE)
	{
		throw DatabaseException("Too big record size! " + std::to_string(m_BytesPerRecord + m_FlagBytesPerRecord) + ">" + std::to_string(PAGE_SIZE));
	}
	m_Pages.clear();
}

void StorageEngine::AddRecord(const Record& record)
{
	// Record's first attribute is the ID (int). [ (int*)((char*)data + 0) <-- id ]

	int index = m_Header.FreeRecordIndex;
	char* data = _GetRecordAt(index);
	char* flag = data + m_BytesPerRecord;
	while (*flag != 0)
	{
		data = _GetRecordAt(++index);
		flag = data + m_BytesPerRecord;
	}
	m_Header.FreeRecordIndex++;

	char* newRecord = RecordsToBytes(record, m_BytesPerRecord);

	memcpy((void*)data, (const void*)newRecord, m_BytesPerRecord);
	*(int*)data = index;
	*flag = 1;

	delete[] newRecord;
}

Record StorageEngine::GetRecord(const Table& table, int id)
{
	char* data = _FindRecordById(id);

	RecordBuilder builder(table, data);

	return builder.GetRecord();
}

std::vector<Record> StorageEngine::GetRecords(const Table& table)
{
	std::vector<Record> records;

	int maxNumRecordsPerPage = PAGE_SIZE / (m_BytesPerRecord + m_FlagBytesPerRecord);
	int numPages = m_Header.NumPages;
	for (int i = 0; i < numPages * maxNumRecordsPerPage; ++i)
	{
		char* record = _GetRecordAt(i);
		char* flag = record + m_BytesPerRecord;
		if (*flag == 1)
		{
			RecordBuilder builder(table, record);
			records.push_back(builder.GetRecord());
		}
	}

	return records;
}

void StorageEngine::Update(const Record& record, int id)
{
	char* data = _FindRecordById(id);

	char* newRecord = RecordsToBytes(record, m_BytesPerRecord);

	memcpy((void*)data, (const void*)newRecord, m_BytesPerRecord);
	*(int*)data = id;

	delete[] newRecord;
}

void StorageEngine::DeleteRecord(int id)
{
	char* data = _FindRecordById(id);
	memset((void*)data, 0, m_BytesPerRecord + m_FlagBytesPerRecord);
	if (id < m_Header.FreeRecordIndex)
	{
		m_Header.FreeRecordIndex = id;
	}
}

char* StorageEngine::_GetRecordAt(int index)
{
	int maxNumRecordsPerPage = PAGE_SIZE / (m_BytesPerRecord + m_FlagBytesPerRecord);
	int PageIndex = index / maxNumRecordsPerPage;
	int IndexInPage = index % maxNumRecordsPerPage;

	Page& page = _LoadPage(PageIndex);

	return page.GetPage() + IndexInPage * (m_BytesPerRecord + m_FlagBytesPerRecord);
}

char* StorageEngine::_FindRecordById(int id)
{
	int maxNumRecordsPerPage = PAGE_SIZE / (m_BytesPerRecord + m_FlagBytesPerRecord);
	int PageIndex = id / maxNumRecordsPerPage;
	int IndexInPage = id % maxNumRecordsPerPage;

	if(PageIndex >= m_Header.NumPages)
		throw StorageEnginePageIndexOutOfBoundException();

	return _GetRecordAt(id);
}

Page& StorageEngine::_LoadPage(int index)
{
	for (Page& page : m_Pages)
	{
		if (page.GetHeader().PageIndex == index)
			return page;
	}

	m_Pages.push_back(Page(PAGE_SIZE));
	Page& newPage = m_Pages[m_Pages.size() - 1];
	newPage.GetHeader().PageIndex = index;

	if (m_Pages.size() > MAX_NUM_PAGES)
	{
		m_Pages.pop_front();
	}

	FileManager* fmanager = FileManager::GetInstance();

	try
	{
		fmanager->ReadPageData(m_TableName, index, newPage.GetPage(), newPage.GetHeader().NumBytes);
		fmanager->ReadBinaryFile(m_TableName, m_TableName + "." + std::to_string(index) + ".header", (char*)(&newPage.GetHeader()), sizeof(PageHeader));
	}
	catch (const FileManagerFileDoesNotExistsException& e)
	{
		m_Header.NumPages++;
	}

	return newPage;
}
