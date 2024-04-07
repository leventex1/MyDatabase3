#pragma once
#include <functional>
#include <string>
#include "Table.h"
#include "DBexceptions.h"
#include <memory>


class Condition;


class Operand
{
public:
	virtual ~Operand() = default;
	virtual void CalulateParams(const Table& table) = 0;
	virtual const char* GetData(const char* data) const = 0;
	virtual ColumnType GetColumnType() const = 0;
};

class ValueOperand : public Operand
{
public:
	ValueOperand(ColumnType type, const std::string& value);
	virtual ~ValueOperand() { delete[] m_Data; }

	virtual void CalulateParams(const Table& table) override { }
	virtual const char* GetData(const char* data) const override { return m_Data; }
	virtual ColumnType GetColumnType() const override { return m_Type; }

private:
	ColumnType m_Type;
	char* m_Data;
};


class ColumnNameOperand : public Operand
{
public:
	ColumnNameOperand(const std::string& columnName);

	virtual void CalulateParams(const Table& table) override;
	virtual const char* GetData(const char* data) const override;
	virtual ColumnType GetColumnType() const override { return m_Type; }

private:
	std::string m_ColumnName;
	ColumnType m_Type;
	int m_Offset;
};


class ConditionOperand : public Operand
{
public:
	ConditionOperand(const std::shared_ptr<Condition>& condition) : m_Condition(condition) { }

	virtual void CalulateParams(const Table& table) override { }
	virtual const char* GetData(const char* data) const override { return nullptr; }
	virtual ColumnType GetColumnType() const override { return ColumnType::None; }

	std::shared_ptr<Condition>& GetCondition() { return m_Condition; }

private:
	std::shared_ptr<Condition> m_Condition;
};


class Condition
{
public:
	Condition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right);
	virtual ~Condition() = default;

	virtual std::function<bool(const char*)> Compile(const Table& table) = 0;

protected:
	void PrepareOperands(const Table& table);

	std::shared_ptr<Operand> m_LeftOperand;
	std::shared_ptr<Operand> m_RightOperand;
};


class EqualCondition : public Condition
{
public:
	EqualCondition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class GreaterThanCondition : public Condition
{
public:
	GreaterThanCondition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class GreaterThanOrEqualCondition : public Condition
{
public:
	GreaterThanOrEqualCondition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class LessThanCondition : public Condition
{
public:
	LessThanCondition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class LessThanOrEqualCondition : public Condition
{
public:
	LessThanOrEqualCondition(const std::shared_ptr<Operand>& left, const std::shared_ptr<Operand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class AndCondition : public Condition
{
public:
	AndCondition(const std::shared_ptr<ConditionOperand>& left, const std::shared_ptr<ConditionOperand>& right) : Condition(left, right) { }
	
	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};


class OrCondition : public Condition
{
public:
	OrCondition(const std::shared_ptr<ConditionOperand>& left, const std::shared_ptr<ConditionOperand>& right) : Condition(left, right) { }

	virtual std::function<bool(const char*)> Compile(const Table& table) override;
};