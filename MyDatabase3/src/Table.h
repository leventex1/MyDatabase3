#pragma once
#include <string>
#include <vector>
#include "Column.h"


class Table
{
public:
	Table(const std::string& name);

	void AddColumn(Column column);

	int GetBytes() const;

	void Print() const;

	inline const std::string& GetName() const { return m_Name; }
	inline const std::vector<Column>& GetColumns() const { return m_Columns; }

private:
	std::string m_Name;
	std::vector<Column> m_Columns;
};