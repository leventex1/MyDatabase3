#include <iostream>
#include "Record.h"


Record::Record() { }

void Record::AddAttr(RecordAttr attr)
{
	for (const RecordAttr& a : m_Attrs)
		if (attr.Name == a.Name)
			throw std::runtime_error("Attribute name already exists on Record!");

	m_Attrs.push_back(attr);
}

void Record::Print() const
{
	for (const RecordAttr& attr : m_Attrs)
	{
		std::cout << attr.Name << " " << ColumnTypeToString(attr.Type) << " " << attr.Value << std::endl;
	}
}