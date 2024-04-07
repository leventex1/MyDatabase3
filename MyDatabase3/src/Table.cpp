#include "Table.h"
#include <iostream>
#include "DBexceptions.h"


Table::Table(const std::string& name)
	: m_Name(name)
{ 
	AddColumn(Column("id", ColumnType::Int));
}

void Table::AddColumn(Column column)
{
	for (const Column& col : m_Columns)
		if (column.GetName() == col.GetName())
			throw DatabaseException("Column with given name already exists!");

	m_Columns.push_back(column);
}

int Table::GetBytes() const
{
	int bytes = 0;
	for (const Column& col : m_Columns)
		bytes += col.GetBytes();

	return bytes;
}

void Table::Print() const
{
	std::cout << m_Name << std::endl;
	for (const Column& column : m_Columns)
	{
		std::cout << column.GetName() << " " << ColumnTypeToString(column.GetType()) << " " << column.GetBytes() << std::endl;
	}
}