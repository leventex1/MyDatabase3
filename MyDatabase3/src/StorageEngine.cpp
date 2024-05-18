#include "StorageEngine.h"
#include "RecordBuilder.h"
#include "FileManager.h"


void StorageEngine::AddRecord(const Record& record)
{
	// Record's first attribute is the ID (int). [ (int*)((char*)data + 0) <-- id ]
	const int bytesPerRecord = GetBytesPerRecord();

	int index = GetHeader().FreeRecordIndex;
	char* data = GetRecordAt(index);
	char* flag = data + bytesPerRecord;
	while (*flag != 0)
	{
		data = GetRecordAt(++index);
		flag = data + bytesPerRecord;
	}
	GetHeader().FreeRecordIndex++;

	char* newRecord = RecordsToBytes(record, bytesPerRecord);

	memcpy((void*)data, (const void*)newRecord, bytesPerRecord);
	*(int*)data = index;
	*flag = 1;

	delete[] newRecord;
}

Record StorageEngine::GetRecord(const Table& table, int id)
{
	char* data = FindRecordById(id);

	RecordBuilder builder(table, data);

	return builder.GetRecord();
}

std::vector<Record> StorageEngine::GetRecords(const Table& table)
{
	std::vector<Record> records;

	int maxNumRecordsPerPage = GetBytesPerPage() / (GetBytesPerRow());
	int numPages = GetHeader().NumPages;
	for (int i = 0; i < numPages * maxNumRecordsPerPage; ++i)
	{
		char* record = GetRecordAt(i);
		char* flag = record + GetBytesPerRecord();
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
	char* data = FindRecordById(id);

	char* newRecord = RecordsToBytes(record, GetBytesPerRecord());

	memcpy((void*)data, (const void*)newRecord, GetBytesPerRecord());
	*(int*)data = id;

	delete[] newRecord;
}

void StorageEngine::DeleteRecord(int id)
{
	char* data = FindRecordById(id);
	memset((void*)data, 0, GetBytesPerRow());
	if (id < GetHeader().FreeRecordIndex)
	{
		GetHeader().FreeRecordIndex = id;
	}
}


InMemoryStorageEngine::InMemoryStorageEngine(int bytesPerRecord, const std::shared_ptr<IPageFactory>& pageFactory)
	: m_PageFactory(pageFactory), m_BytesPerRecord(bytesPerRecord)
{
	m_BytesPerRow = bytesPerRecord + 1;  // 1 flag byte.
}

InMemoryStorageEngine::~InMemoryStorageEngine()
{
}

void InMemoryStorageEngine::Save() const
{
}

void InMemoryStorageEngine::Load()
{
}

char* InMemoryStorageEngine::GetRecordAt(int index)
{
	int maxNumRecordsPerPage = GetBytesPerPage() / (GetBytesPerRow());
	int PageIndex = index / maxNumRecordsPerPage;
	int IndexInPage = index % maxNumRecordsPerPage;

	std::unique_ptr<IPage>& page = LoadPage(PageIndex);

	return page->GetData() + IndexInPage * (GetBytesPerRow());
}

char* InMemoryStorageEngine::FindRecordById(int id)
{
	int maxNumRecordsPerPage = GetBytesPerPage() / (GetBytesPerRow());
	int PageIndex = id / maxNumRecordsPerPage;
	int IndexInPage = id % maxNumRecordsPerPage;

	if(PageIndex >= GetHeader().NumPages)
		throw StorageEnginePageIndexOutOfBoundException();

	std::unique_ptr<IPage>& page = LoadPage(PageIndex);

	return page->GetData() + IndexInPage * (GetBytesPerRow());
}

std::unique_ptr<IPage>& InMemoryStorageEngine::LoadPage(int index)
{
	for (std::unique_ptr<IPage>& page : m_Pages)
	{
		if (page->GetHeader().PageIndex == index)
			return page;
	}

	m_PageFactory->ConstructPage();
	m_Pages.push_back(m_PageFactory->GetPage());
	std::unique_ptr<IPage>& newPage = m_Pages[m_Pages.size() - 1];
	newPage->GetHeader().PageIndex = index;

	GetHeader().NumPages++;

	return newPage;
}
