#include "Condition.h"
#include "RecordBuilder.h"


ValueOperand::ValueOperand(ColumnType type, const std::string& value) 
	: m_Type(type)
{
	m_Data = RecordAttrToByteData({ "", type, value });
}

ColumnNameOperand::ColumnNameOperand(const std::string& columnName)
	: m_ColumnName(columnName), m_Type(ColumnType::None), m_Offset(-1)
{
}

void ColumnNameOperand::CalulateParams(const Table& table)
{
	int offset = 0;
	for (const Column& column : table.GetColumns())
	{
		if (column.GetName() == m_ColumnName)
		{
			m_Offset = offset;
			m_Type = column.GetType();
			return;
		}
		offset += column.GetBytes();
	}

	throw ConditionException("Cannot create condition because, column name ("+m_ColumnName+") does not exists on table ("+table.GetName()+")!");
}

const char* ColumnNameOperand::GetData(const char* data) const
{
	return data + m_Offset;
}


Condition::Condition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right)
	: m_LeftOperand(left), m_RightOperand(right)
{
}

void Condition::PrepareOperands(const Table& table)
{
	m_LeftOperand->CalulateParams(table);
	m_RightOperand->CalulateParams(table);

	// Check if the two operand are the same type (int, string, text).
	if (m_LeftOperand->GetColumnType() != m_RightOperand->GetColumnType())
		throw ConditionException("Condition cannot compile because the two operand are not the same type!");
}


std::function<bool(const char*)> EqualCondition::Compile(const Table& table)
{
	PrepareOperands(table);

	return [this](const char* data) -> bool
	{
		ColumnType type = m_LeftOperand->GetColumnType();
		const char* left = m_LeftOperand->GetData(data);
		const char* right = m_RightOperand->GetData(data);

		return CompareEqual(type, left, right);
	};
}


std::function<bool(const char*)> GreaterThanCondition::Compile(const Table& table)
{
	PrepareOperands(table);

	return [this](const char* data) -> bool
	{
		ColumnType type = m_LeftOperand->GetColumnType();
		const char* left = m_LeftOperand->GetData(data);
		const char* right = m_RightOperand->GetData(data);

		return CompareGreatherThan(type, left, right);
	};
}


std::function<bool(const char*)> GreaterThanOrEqualCondition::Compile(const Table& table)
{
	PrepareOperands(table);

	return [this](const char* data) -> bool
	{
		ColumnType type = m_LeftOperand->GetColumnType();
		const char* left = m_LeftOperand->GetData(data);
		const char* right = m_RightOperand->GetData(data);

		return CompareGreatherThanOrEqual(type, left, right);
	};
}


std::function<bool(const char*)> LessThanCondition::Compile(const Table& table)
{
	PrepareOperands(table);

	return [this](const char* data) -> bool
	{
		ColumnType type = m_LeftOperand->GetColumnType();
		const char* left = m_LeftOperand->GetData(data);
		const char* right = m_RightOperand->GetData(data);

		return CompareLessThan(type, left, right);
	};
}


std::function<bool(const char*)> LessThanOrEqualCondition::Compile(const Table& table)
{
	PrepareOperands(table);

	return [this](const char* data) -> bool
	{
		ColumnType type = m_LeftOperand->GetColumnType();
		const char* left = m_LeftOperand->GetData(data);
		const char* right = m_RightOperand->GetData(data);

		return CompareLessThanOrEqual(type, left, right);
	};
}


std::function<bool(const char*)> AndCondition::Compile(const Table& table)
{
	std::shared_ptr<ConditionOperand> leftOperand = std::dynamic_pointer_cast<ConditionOperand>(m_LeftOperand);
	std::shared_ptr<ConditionOperand> rightOperand = std::dynamic_pointer_cast<ConditionOperand>(m_RightOperand);

	std::function<bool(const char*)> leftCompiledCondition = leftOperand->GetCondition()->Compile(table);
	std::function<bool(const char*)> rightCompiledCondition = rightOperand->GetCondition()->Compile(table);

	return [leftCompiledCondition, rightCompiledCondition](const char* data) -> bool
	{
		return leftCompiledCondition(data) && rightCompiledCondition(data);
	};
}



std::function<bool(const char*)> OrCondition::Compile(const Table& table)
{
	std::shared_ptr<ConditionOperand> leftOperand = std::dynamic_pointer_cast<ConditionOperand>(m_LeftOperand);
	std::shared_ptr<ConditionOperand> rightOperand = std::dynamic_pointer_cast<ConditionOperand>(m_RightOperand);

	std::function<bool(const char*)> leftCompiledCondition = leftOperand->GetCondition()->Compile(table);
	std::function<bool(const char*)> rightCompiledCondition = rightOperand->GetCondition()->Compile(table);

	return [leftCompiledCondition, rightCompiledCondition](const char* data) -> bool
	{
		return leftCompiledCondition(data) || rightCompiledCondition(data);
	};
}