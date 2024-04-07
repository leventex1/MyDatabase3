#include "Column.h"
#include <sstream>
#include "DBexceptions.h"


Column::Column(const std::string& name, ColumnType type)
	: m_Name(name), m_ColumnType(type), m_Bytes(ColumnTypeToBytes(type))
{
}

Column::Column(const std::string& str)
{
	FromString(str);
}

std::string Column::ToString() const
{
	std::stringstream ss;

	ss << m_Name << " " << (int)m_ColumnType << " " << m_Bytes;

	return ss.str();
}

void Column::FromString(const std::string& str)
{
	std::stringstream ss(str);

	if(!(ss >> m_Name))
		throw DatabaseException("Column from string: name does not exists!");

	try
	{
		int type = -1;
		ss >> type;
		m_ColumnType = (ColumnType)type;
	}
	catch (...)
	{
		throw DatabaseException("Column from string: cannot convert from string to int to ColumnType!");
	}

	if(!(ss >> m_Bytes))
		throw DatabaseException("Column from string: byte does not exists!");

}