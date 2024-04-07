#pragma once
#include <string>
#include "DBexceptions.h"


enum ColumnType
{
	None = 0,
	String, Int, Text
};

static std::string ColumnTypeToString(ColumnType type)
{
	switch (type)
	{
		case String:	return "String";
		case Int:		return "Int";
		case Text:		return "Text";
	}

	throw DatabaseException("Unknown column type!");
	return "None";
}

static int ColumnTypeToBytes(ColumnType type)
{
	switch (type)
	{
		case String:	return 256	* sizeof(char);
		case Int:		return 1	* sizeof(int);
		case Text:		return 2048 * sizeof(char);
	}

	throw DatabaseException("Unknown column type!");
	return 0;
}

class Column
{
public:
	Column(const std::string& name, ColumnType type);
	Column(const std::string& fromString);

	inline const std::string& GetName() const { return m_Name; }
	inline ColumnType GetType() const { return m_ColumnType; }
	inline int GetBytes() const { return m_Bytes; }

	std::string ToString() const;
	void FromString(const std::string& str);

private:
	std::string m_Name;
	ColumnType m_ColumnType;
	int m_Bytes;
};
