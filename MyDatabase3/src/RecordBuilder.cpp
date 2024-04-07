#include "RecordBuilder.h"
#include "DBexceptions.h"


char* RecordAttrToByteData(const RecordAttr& attr)
{
	int attrBytes = ColumnTypeToBytes(attr.Type);
	char* data = new char[attrBytes];

	switch (attr.Type)
	{
	case Int:
	{
		try
		{
			int attrData = std::stoi(attr.Value);
			*(int*)data = attrData;
		}
		catch (...)
		{
			throw RecordBuilderException("Can't convert type int! " + attr.Value);
		}
		break;
	}
	case String:
	{
		if (attr.Value.size() >= attrBytes)
			throw RecordBuilderException("String size cannot exceed " + std::to_string(attrBytes) + " bytes!");

		memcpy((void*)data, (const void*)attr.Value.c_str(), attrBytes);
		break;
	}
	case Text:
	{
		if (attr.Value.size() >= attrBytes)
			throw RecordBuilderException("Text size cannot exceed " + std::to_string(attrBytes) + " bytes!");

		memcpy((void*)data, (const void*)attr.Value.c_str(), attrBytes);
		break;
	}
	default:
		throw RecordBuilderException("Unknown Record attribute type!");
	}

	return data;
}

RecordAttr RawDataToRecordAttr(char* data, ColumnType type)
{
	int attrBytes = ColumnTypeToBytes(type);
	RecordAttr attr;
	attr.Type = type;

	switch (type)
	{
	case Int:
	{
		int value = *(int*)data;
		attr.Value = std::to_string(value);
		break;
	}
	case String:
	case Text:
	{
		char* str = new char[attrBytes];
		memcpy(str, data, attrBytes);
		str[attrBytes - 1] = '\0';
		attr.Value = std::string(str);
		delete[] str;
		break;
	}
	default:
		throw RecordBuilderException("Unknown Record attribute type!");
	}

	return attr;
}

char* RecordsToBytes(const Record& record, int bytes)
{
	char* data = new char[bytes];

	int offset = 0;
	for (const RecordAttr& attr : record.GetAttrs())
	{
		if (offset >= bytes)
			throw RecordBuilderException("Record can't fit inside the given byte array of " + std::to_string(bytes) + " bytes!");

		int attrBytes = ColumnTypeToBytes(attr.Type);
		char* attrData = RecordAttrToByteData(attr);

		memcpy((void*)(data + offset), (const void*)(attrData), attrBytes);

		delete[] attrData;
		offset += attrBytes;
	}

	return data;
}

RecordBuilder::RecordBuilder(const Table& table)
{
	for (const Column& column : table.GetColumns())
	{
		m_Record.AddAttr({ column.GetName(), column.GetType() , "" });
	}
}
RecordBuilder::RecordBuilder(const Table& table, const Record& copy, int id)
{
	const auto& columns = table.GetColumns();
	const auto& attrs = copy.GetAttrs();

	if (attrs.size() >= columns.size())
		throw RecordBuilderException("Can't populate record from table and copy record, because columns not match!");

	for(int i = 0; i < columns.size(); i++)
	{
		if (i == 0)
		{
			m_Record.AddAttr({ columns[i].GetName(), columns[i].GetType(), std::to_string(id) });
		}
		else
		{
			if (columns[i].GetName() != attrs[i - 1].Name || columns[i].GetType() != attrs[i - 1].Type)
				throw RecordBuilderException("Can't populate record from table and copy record, because column attributes not match!");
				
			m_Record.AddAttr({ columns[i].GetName(), columns[i].GetType(), attrs[i - 1].Value });
		}
	}
}

RecordBuilder::RecordBuilder(const Table& table, char* rawData)
{
	int offset = 0;
	for (const Column& column : table.GetColumns())
	{
		int bytes = column.GetBytes();
		char* data = rawData + offset;

		RecordAttr attr = RawDataToRecordAttr(data, column.GetType());
		attr.Name = column.GetName();
		m_Record.AddAttr(attr);

		offset += bytes;
	}
}

bool CompareEqual(ColumnType type, const char* left, const char* right)
{
	switch (type)
	{
	case Int:		return *(int*)left == *(int*)right;
	case String:	return (std::string(left) == std::string(right));
	case Text:		return (std::string(left) == std::string(right));
	}

	throw RecordBuilderException("Unknown Record attribute type!");
}

bool CompareGreatherThan(ColumnType type, const char* left, const char* right)
{
	switch (type)
	{
	case Int:		return *(int*)left > *(int*)right;
	case String:	return (std::string(left) > std::string(right));
	case Text:		return (std::string(left) > std::string(right));
	}

	throw RecordBuilderException("Unknown Record attribute type!");
}

bool CompareGreatherThanOrEqual(ColumnType type, const char* left, const char* right)
{
	switch (type)
	{
	case Int:		return *(int*)left >= *(int*)right;
	case String:	return (std::string(left) >= std::string(right));
	case Text:		return (std::string(left) >= std::string(right));
	}

	throw RecordBuilderException("Unknown Record attribute type!");
}

bool CompareLessThan(ColumnType type, const char* left, const char* right)
{
	switch (type)
	{
	case Int:		return *(int*)left < *(int*)right;
	case String:	return (std::string(left) < std::string(right));
	case Text:		return (std::string(left) < std::string(right));
	}

	throw RecordBuilderException("Unknown Record attribute type!");
}

bool CompareLessThanOrEqual(ColumnType type, const char* left, const char* right)
{
	switch (type)
	{
	case Int:		return *(int*)left <= *(int*)right;
	case String:	return (std::string(left) <= std::string(right));
	case Text:		return (std::string(left) <= std::string(right));
	}

	throw RecordBuilderException("Unknown Record attribute type!");
}