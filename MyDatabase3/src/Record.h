#pragma once
#include <vector>
#include <string>
#include "Column.h"


struct RecordAttr
{
	std::string Name;
	ColumnType Type;
	std::string Value;
};

class Record
{
public:
	Record();

	void AddAttr(RecordAttr attr);

	void Print() const;

	inline const std::vector<RecordAttr>& GetAttrs() const { return m_Attrs; }
	inline std::vector<RecordAttr>& GetAttrs() { return m_Attrs; }

private:
	std::vector<RecordAttr> m_Attrs;
};