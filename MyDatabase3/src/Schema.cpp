#include "Schema.h"
#include <sstream>
#include "DBexceptions.h"


Schema::Schema(const std::string& name)
	: m_Name(name) { }

void Schema::AddTable(Table table)
{
	for (const Table& t: m_Tables)
		if (t.GetName() == table.GetName())
			throw DatabaseException("Table with given name already exists! " + t.GetName());

	m_Tables.push_back(table);
}

void Schema::Print() const
{
	std::cout << "+---- " << m_Name << " ----+\n";

	for (const Table& table : m_Tables)
	{
		std::cout << std::endl << "--- ";
		table.Print();
	}

	std::cout << std::endl;
}

std::string Schema::ToString() const
{
	std::stringstream ss;

	ss << m_Name << "\n";

	for (const Table& table : m_Tables)
	{
		ss << "\t" << table.GetName() << "\n";
		for (const Column& column : table.GetColumns())
		{
			ss << "\t\t" << column.ToString() << "\n";
		}
		ss << ";\n";
	}

	return ss.str();
}

bool Schema::_ReadTable(std::stringstream& ss)
{
	std::string value;
	if (!(ss >> value))
		return false;

	Table table(value);

	while (std::getline(ss, value))
	{
		if (value.empty())
			continue;

		if (value == ";")
			break;

		Column column(value);
		if (column.GetName() == "id")  // Id column gets added first when the table is created.
			continue;

		table.AddColumn(column);
	}

	AddTable(table);
	return true;
}

void Schema::FromString(const std::string& str)
{
	std::stringstream ss(str);

	ss >> m_Name;
	while (_ReadTable(ss));
}

bool Schema::Match(const Schema& other) const
{
	return ToString() == other.ToString();
}