#pragma once
#include <string>
#include <memory>
#include "Record.h"
#include "Schema.h"
#include "StorageManager.h"
#include "Condition.h"


/*
* 
				Database
				   |
				 Schema
				/  |  \
		  Table1  t2  t3...						Record
		 /  |  \							   /  |  \
  Column1  c2  c3...				RecordAttr1  RA2  RA3...

	
	Table	~= Record		
	Column	~= RecordAttr	

	Table:			Holds the Name and the columns of the table.
	Column:			Holds the Name, Type, Bytes (later constrains) of the columns.
	Record:			Holds the RecordAttributes.
	RecordAttr:		Holds the Name, Type, Value of the Record's fields.
*/

class Database
{
public:
	Database(const Schema& schema, const std::shared_ptr<StorageManager>& storageManager);

	void Commit();
	void DropAll();

	void InsertInto(const std::string& tableName, const Record& record);

	std::vector<Record> Select(const std::string& tableName);
	Record Select(const std::string& tableName, int id);
	std::vector<Record> SelectWhere(const std::string& tableName, const std::shared_ptr<Condition> condition);

	/*
		Record is a raw record, it does not contain the id attr (first attr).
	*/
	void Update(const std::string& tableName, const Record& record, int id);
	/*
		Record contain the id attr (first attr).
	*/
	void Update(const std::string& tableName, const Record& record);

	void Delete(const std::string& tableName, int id);

	inline const Schema& GetSchama() const { return m_Schema; }
	inline void SetSchema(const Schema& schema) { m_Schema = schema; }

	inline const std::shared_ptr<StorageManager>& GetStorageManager() const { return m_StorageManager; }
	inline void SetStorageManager(const std::shared_ptr<StorageManager>& smanager) { m_StorageManager = smanager; }

private:
	const Table* _GetTable(const std::string& tableName) const;

private:
	Schema m_Schema;
	std::shared_ptr<StorageManager> m_StorageManager;
};