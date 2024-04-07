#pragma once
#include "Record.h"
#include "Table.h"


class RecordBuilder
{
public:
	RecordBuilder(const Table& table);
	RecordBuilder(const Table& table, const Record& copy, int id);
	RecordBuilder(const Table& table, char* rawData);

	Record GetRecord() const { return m_Record; }
private:
	Record m_Record;
};


char* RecordAttrToByteData(const RecordAttr& attr);
char* RecordsToBytes(const Record& record, int bytes);

bool CompareEqual(ColumnType type, const char* left, const char* right);
bool CompareGreatherThan(ColumnType type, const char* left, const char* right);
bool CompareGreatherThanOrEqual(ColumnType type, const char* left, const char* right);
bool CompareLessThan(ColumnType type, const char* left, const char* right);
bool CompareLessThanOrEqual(ColumnType type, const char* left, const char* right); 