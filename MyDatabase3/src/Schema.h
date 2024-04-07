#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Table.h"


class Schema
{
public:
	Schema() { }
	Schema(const std::string& name);

	void AddTable(Table table);

	void Print() const;

	inline const std::vector<Table>& GetTables() const { return m_Tables; }
	inline const std::string& GetName() const { return m_Name; }

	std::string ToString() const;
	void FromString(const std::string& str);

	bool Match(const Schema& other) const;

private:
	bool _ReadTable(std::stringstream& ss);

private:
	std::string m_Name;
	std::vector<Table> m_Tables;
};