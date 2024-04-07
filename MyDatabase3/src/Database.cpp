#include "Database.h"
#include "DBexceptions.h"
#include "StorageManager.h"


Database::Database(const Schema& schema)
	: m_Schema(schema)
{ 
	m_StorageManager.BuildStore(schema);
}

Database::Database(const std::string& dbName)
{
	m_Schema = m_StorageManager.BuildStore(dbName);
}

const Table* Database::_GetTable(const std::string& tableName) const
{
	const Table* table = nullptr;
	for (const Table& t : m_Schema.GetTables())
		if (t.GetName() == tableName)
			table = &t;

	if (!table)
		throw InsertionException("No table found with name: " + tableName);

	return table;
}

void Database::Commit()
{
	m_StorageManager.Commit();
}

void Database::DropAll()
{
	m_StorageManager.DropAll();
}

void Database::InsertInto(const std::string& tableName, const Record& record)
{
	const Table* table = _GetTable(tableName);

	const std::vector<Column>& columns = table->GetColumns();
	const std::vector<RecordAttr>& attrs = record.GetAttrs();

	if (attrs.size() >= columns.size())
		throw InsertionException("Table and record columns not match!");

	for (int i = 0; i < attrs.size(); ++i)
	{
		const Column& column = columns[columns.size() - 1 - i];
		const RecordAttr& attr = attrs[attrs.size() - 1 - i];

		if (column.GetName() != attr.Name || column.GetType() != attr.Type)
			throw InsertionException("Table and record column attributes not match!");
	}

	m_StorageManager.Insert(*table, record);
}

std::vector<Record> Database::Select(const std::string& tableName)
{
	const Table* table = _GetTable(tableName);

	return m_StorageManager.Select(*table);
}

Record Database::Select(const std::string& tableName, int id)
{
	const Table* table = _GetTable(tableName);

	return m_StorageManager.Select(*table, id);
}

std::vector<Record> Database::SelectWhere(const std::string& tableName, const std::shared_ptr<Condition> condition)
{
	const Table* table = _GetTable(tableName);

	auto compiledCondition = condition->Compile(*table);

	return { };
}

void Database::Update(const std::string& tableName, const Record& record, int id)
{
	const Table* table = _GetTable(tableName);
	
	const std::vector<Column>& columns = table->GetColumns();
	const std::vector<RecordAttr>& attrs = record.GetAttrs();

	if (attrs.size() >= columns.size())
		throw InsertionException("Table and record columns not match!");

	for (int i = 0; i < attrs.size(); ++i)
	{
		const Column& column = columns[columns.size() - 1 - i];
		const RecordAttr& attr = attrs[attrs.size() - 1 - i];

		if (column.GetName() != attr.Name || column.GetType() != attr.Type)
			throw InsertionException("Table and record column attributes not match!");
	}

	m_StorageManager.Update(*table, record, id);
}

void Database::Update(const std::string& tableName, const Record& record)
{
	const Table* table = _GetTable(tableName);

	const std::vector<Column>& columns = table->GetColumns();
	const std::vector<RecordAttr>& attrs = record.GetAttrs();

	if (attrs.size() != columns.size())
		throw InsertionException("Table and record columns not match!");

	for (int i = 0; i < attrs.size(); ++i)
	{
		const Column& column = columns[columns.size() - 1 - i];
		const RecordAttr& attr = attrs[attrs.size() - 1 - i];

		if (column.GetName() != attr.Name || column.GetType() != attr.Type)
			throw InsertionException("Table and record column attributes not match!");
	}

	m_StorageManager.Update(*table, record);
}

void Database::Delete(const std::string& tableName, int id)
{
	const Table* table = _GetTable(tableName);
	m_StorageManager.Delete(*table, id);
}